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
	
public:
	PacketConsumer(){}
	
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
		_ipa *padded_ip = reinterpret_cast<_ipa*>(&ip_addr);
		dbg_printf("%d.%d.%d.%d", padded_ip->a, padded_ip->b, padded_ip->c, padded_ip->d);
	}
	
	void pluginsIPv6(ip6_hdr *data){
		
	}
	
	void pluginsTCP(tcphdr *data){
		dbg_printf(":%d", ntohs(data->dest));
	}
	
	void pluginsUDP(udphdr *data){
		
	}
	
	void pluginsAPP(uint8_t *data){
		
	}
	
	void parse(RawPacketElem *data){
		uint8_t *tcp_layer;
		iphdr *ip_layer = reinterpret_cast<iphdr*>(data->getData());
		uint8_t protocol;
		
		switch (ip_layer->version){
			case IPV4_VER:
				tcp_layer = reinterpret_cast<uint8_t*>(ip_layer) + (ip_layer->ihl * 4);
				protocol = ip_layer->protocol;
				pluginsIPv4(ip_layer);
				break;
			case IPV6_VER:
				tcp_layer = reinterpret_cast<uint8_t*>(ip_layer) + sizeof(ip6_hdr);
				protocol = reinterpret_cast<ip6_hdr*>(ip_layer)->ip6_ctlun.ip6_un1.ip6_un1_nxt;
				pluginsIPv6(reinterpret_cast<ip6_hdr*>(ip_layer));
				break;
			default:
				tcp_layer = nullptr;
				protocol = 0;
				break;
		}
		if (tcp_layer == nullptr || protocol == 0) return;
		
		uint8_t *app_layer;
		
		switch (protocol){
			case IPPROTO_TCP:
				app_layer = reinterpret_cast<uint8_t*>(tcp_layer) + (reinterpret_cast<tcphdr*>(tcp_layer)->doff * 4);
				pluginsTCP(reinterpret_cast<tcphdr*>(tcp_layer));
				break;
			case IPPROTO_UDP:
				app_layer = reinterpret_cast<uint8_t*>(tcp_layer) + sizeof(udphdr);
				pluginsUDP(reinterpret_cast<udphdr*>(tcp_layer));
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
			dbg_printf("\n");
		}
	}
	
};
