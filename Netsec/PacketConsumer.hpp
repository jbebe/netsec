#pragma once

#include <mutex>
#include <condition_variable>
#include <chrono>
#include <cstdint>

#include <netinet/ip.h>
#include <netinet/ip6.h>
#include <netinet/tcp.h>
#include <netinet/udp.h>

#include "../debug.hpp"
#include "../Globals.hpp"
#include "../CoPro/MTStack.hpp"
#include "RawPacketElem.hpp"
#include "ParsedPacketElem.hpp"

#define IPV4_VER 4
#define IPV6_VER 6

template <int Tcapacity>
class PacketConsumer {

	// consumer properties
	MTStack<RawPacketElem, Tcapacity> queue;
	std::mutex cond_mut;
	std::condition_variable cond;
	
	// plugins
	ParsedPacketElem parsed_elem;
	
	// debug
	std::stringstream dbg_str;
	
public:
	PacketConsumer(): dbg_str{} {}
	
	PacketConsumer(PacketConsumer &&moved_consumer): PacketConsumer() {}
	
	PacketConsumer(const PacketConsumer &) = delete;

	void put(RawPacketElem *data){
		queue.put(data);
	}
	
	bool try_put(RawPacketElem *data){
		return queue.try_put(data);
	}
	
	void get(RawPacketElem *data_in){
		queue.get(data_in);
	}
	
	void pluginsIPv4(iphdr *data){
		uint32_t ip_addr = ntohl(data->saddr);
		struct _ipa { uint8_t d, c, b, a; };
		_ipa *padded_ip = (_ipa*)(&ip_addr);
		dbg_str << 1*(padded_ip->a) << "." << 1*(padded_ip->b) << "." << 1*(padded_ip->c) << "." << 1*(padded_ip->d);
	}
	
	void pluginsIPv6(ip6_hdr *data){
		
	}
	
	void pluginsTCP(tcphdr *data){
		dbg_str << ":" << ntohs(data->dest);
	}
	
	void pluginsUDP(udphdr *data){
		
	}
	
	void pluginsAPP(uint8_t *data){
		dbg_str << " - ";
		for (int i = 0; i < 40; i++){
			char out;
			if (data[i] >= 32 && data[i] <= 126){
				out = data[i];
			}
			else {
				out = '.';
			}
			dbg_str << out;
		}
	}
	
	void parse(RawPacketElem *data){
		uint8_t *tcp_layer;
		iphdr *ip_layer = (iphdr*)(data->getData());
		uint8_t protocol;
		
		switch (ip_layer->version){
			case IPV4_VER:
				tcp_layer = (uint8_t*)ip_layer + (ip_layer->ihl * 4);
				protocol = ip_layer->protocol;
				pluginsIPv4(ip_layer);
				break;
			case IPV6_VER:
				tcp_layer = (uint8_t*)ip_layer + sizeof(ip6_hdr);
				protocol = ((ip6_hdr*)ip_layer)->ip6_ctlun.ip6_un1.ip6_un1_nxt;
				pluginsIPv6((ip6_hdr*)ip_layer);
				break;
			default:
				tcp_layer = nullptr;
				protocol = 0;
				break;
		}
		if (protocol == 0 || tcp_layer == nullptr) return;
		
		uint8_t *app_layer;
		
		switch (protocol){
			case IPPROTO_TCP:
				app_layer = (uint8_t*)tcp_layer + (((tcphdr*)tcp_layer)->doff * 4);
				pluginsTCP((tcphdr*)tcp_layer);
				break;
			case IPPROTO_UDP:
				app_layer = (uint8_t*)tcp_layer + sizeof(udphdr);
				pluginsUDP((udphdr*)tcp_layer);
				break;
			default: 
				app_layer = nullptr;
				break;
		}
		if (app_layer == nullptr) return;
		
		pluginsAPP(app_layer);
	}	
	
	void run(){
		while (1){
			RawPacketElem data;
			get(&data);
			parse(&data);
			
			dbg_printf("%s\n", dbg_str.str().c_str());
			dbg_str.str(std::string{});
			dbg_str.clear();
		}
	}
	
};

