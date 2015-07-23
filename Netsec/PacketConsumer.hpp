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
#include "StatsBuffer.hpp"

#define IPV4_VER 4
#define IPV6_VER 6

template <int Tcapacity>
class PacketConsumer {

	// consumer properties
	MTStack<RawPacketElem, Tcapacity> queue;
	std::mutex cond_mut;
	std::condition_variable cond;
	
	// stats buffer ptr
	StatsBuffer *stats_data_ptr;
	
	// plugins
	template <typename Theader>
	using parser_fn = std::function<void(Theader*, ParsedPacketElem*)>;
	parser_fn<iphdr> plugin_ipv4;
	parser_fn<ip6_hdr> plugin_ipv6;
	parser_fn<tcphdr> plugin_tcp;
	parser_fn<udphdr> plugin_udp;
	parser_fn<uint8_t> plugin_app;
	
public:
	PacketConsumer(
		StatsBuffer *sb_ptr,
		parser_fn<iphdr> p_ipv4 = parser_fn<iphdr>{},
		parser_fn<ip6_hdr> p_ipv6 = parser_fn<ip6_hdr>{},
		parser_fn<tcphdr> p_tcp = parser_fn<tcphdr>{}, 
		parser_fn<udphdr> p_udp = parser_fn<udphdr>{},
		parser_fn<uint8_t> p_app = parser_fn<uint8_t>{}
	)
	: stats_data_ptr{sb_ptr},
		plugin_ipv4{p_ipv4}, plugin_ipv6{p_ipv6}, plugin_tcp{p_tcp}, 
		plugin_udp{p_udp}, plugin_app{p_app}
	{}
	
	PacketConsumer(PacketConsumer &&moved_consumer)
	: PacketConsumer(moved_consumer.stats_data_ptr,
		moved_consumer.plugin_ipv4,
		moved_consumer.plugin_ipv6,
		moved_consumer.plugin_tcp,
		moved_consumer.plugin_udp,
		moved_consumer.plugin_app
		)
	{}
	
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
	
	void parse(RawPacketElem *raw_data, ParsedPacketElem *parsed_data){
		uint8_t *tcp_layer;
		iphdr *ip_layer = (iphdr*)(raw_data->getData());
		uint8_t protocol;
		
		switch (ip_layer->version){
			case IPV4_VER:
				tcp_layer = (uint8_t*)ip_layer + (ip_layer->ihl * 4);
				protocol = ip_layer->protocol;
				plugin_ipv4(ip_layer, parsed_data);
				break;
			case IPV6_VER:
				tcp_layer = (uint8_t*)ip_layer + sizeof(ip6_hdr);
				protocol = ((ip6_hdr*)ip_layer)->ip6_ctlun.ip6_un1.ip6_un1_nxt;
				plugin_ipv6((ip6_hdr*)ip_layer, parsed_data);
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
				plugin_tcp((tcphdr*)tcp_layer, parsed_data);
				break;
			case IPPROTO_UDP:
				app_layer = (uint8_t*)tcp_layer + sizeof(udphdr);
				plugin_udp((udphdr*)tcp_layer, parsed_data);
				break;
			default: 
				app_layer = nullptr;
				break;
		}
		if (app_layer == nullptr) return;
		
		plugin_app(app_layer, parsed_data);
	}	
	
	void run(){
		while (1){
			RawPacketElem raw_data;
			ParsedPacketElem parsed_data;
			
			get(&raw_data);
			parse(&raw_data, &parsed_data);
			stats_data_ptr->put(&(parsed_data.ip_layer.src_addr), &parsed_data);
			// put -> get 
		}
	}
	
};

