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
	ParsedPacketElem parsed_elem;
	
	// plugins
	template <typename Theader>
	using parser_func = std::function<void(Theader*, ParsedPacketElem*)>;
	parser_func<iphdr> plugin_ipv4;
	parser_func<ip6_hdr> plugin_ipv6;
	parser_func<tcphdr> plugin_tcp;
	parser_func<udphdr> plugin_udp;
	parser_func<uint8_t> plugin_app;
	
public:
	PacketConsumer(
		parser_func<iphdr> p_ipv4 = parser_func<iphdr>{},
		parser_func<ip6_hdr> p_ipv6 = parser_func<ip6_hdr>{},
		parser_func<tcphdr> p_tcp = parser_func<tcphdr>{}, 
		parser_func<udphdr> p_udp = parser_func<udphdr>{},
		parser_func<uint8_t> p_app = parser_func<uint8_t>{}
	)
	: plugin_ipv4{p_ipv4}, plugin_ipv6{p_ipv6}, plugin_tcp{p_tcp}, 
		plugin_udp{p_udp}, plugin_app{p_app}
	{}
	
	PacketConsumer(PacketConsumer &&moved_consumer)
	: PacketConsumer()
	{
		plugin_ipv4 = moved_consumer.plugin_ipv4;
		plugin_ipv6 = moved_consumer.plugin_ipv6;
		plugin_tcp = moved_consumer.plugin_tcp;
		plugin_udp = moved_consumer.plugin_udp;
		plugin_app = moved_consumer.plugin_app;
	}
	
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
	
	void parse(RawPacketElem *data){
		uint8_t *tcp_layer;
		iphdr *ip_layer = (iphdr*)(data->getData());
		uint8_t protocol;
		
		switch (ip_layer->version){
			case IPV4_VER:
				tcp_layer = (uint8_t*)ip_layer + (ip_layer->ihl * 4);
				protocol = ip_layer->protocol;
				plugin_ipv4(ip_layer, &parsed_elem);
				break;
			case IPV6_VER:
				tcp_layer = (uint8_t*)ip_layer + sizeof(ip6_hdr);
				protocol = ((ip6_hdr*)ip_layer)->ip6_ctlun.ip6_un1.ip6_un1_nxt;
				plugin_ipv6((ip6_hdr*)ip_layer, &parsed_elem);
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
				plugin_tcp((tcphdr*)tcp_layer, &parsed_elem);
				break;
			case IPPROTO_UDP:
				app_layer = (uint8_t*)tcp_layer + sizeof(udphdr);
				plugin_udp((udphdr*)tcp_layer, &parsed_elem);
				break;
			default: 
				app_layer = nullptr;
				break;
		}
		if (app_layer == nullptr) return;
		
		plugin_app(app_layer, &parsed_elem);
	}	
	
	void run(){
		while (1){
			RawPacketElem data;
			get(&data);
			parse(&data);
			
		}
	}
	
};

