#pragma once

#include <functional>
#include <mutex>
#include <condition_variable>
#include <chrono>
#include <cstdint>

#include <netinet/ip.h>
#include <netinet/ip6.h>
#include <netinet/tcp.h>
#include <netinet/udp.h>

#include "../../debug.hpp"
#include "../../Globals.hpp"
#include "../../CoPro/MTStack.hpp"
#include "../RawPacketElem.hpp"
#include "../ParsedPacketElem.hpp"
#include "Evaluator.hpp"

#define IPV4_VER 4
#define IPV6_VER 6

class PacketConsumer {
	
	// consumer properties
	MTStack<RawPacketElem, CONSUMER_BUFFER_SIZE> queue;
	std::mutex cond_mut;
	std::condition_variable cond;
	
	// stats buffer ptr
	Evaluator *stats;
	
public:
	struct PluginPack {
		template <typename Theader>
		using parser_fn = std::function<bool(Theader*, ParsedPacketElem*)>;
		parser_fn<iphdr> ipv4;
		parser_fn<ip6_hdr> ipv6;
		parser_fn<tcphdr> tcp;
		parser_fn<udphdr> udp;
		parser_fn<uint8_t> app;
		
		PluginPack(
			parser_fn<iphdr> ipv4 = parser_fn<iphdr>{},
			parser_fn<ip6_hdr> ipv6 = parser_fn<ip6_hdr>{},
			parser_fn<tcphdr> tcp = parser_fn<tcphdr>{},
			parser_fn<udphdr> udp = parser_fn<udphdr>{},
			parser_fn<uint8_t> app = parser_fn<uint8_t>{}
		)
		: ipv4{ipv4}, ipv6{ipv6}, tcp{tcp}, udp{udp}, app{app} {}
	};

private:
	// plugins
	PluginPack plugins;
	
public:
	PacketConsumer(
		Evaluator *stats_ptr = nullptr,
		PluginPack plugins = PluginPack{}
	)
	: stats{stats_ptr}, plugins{plugins}
	{}
	
	PacketConsumer(PacketConsumer &&moved_consumer)
	: PacketConsumer(moved_consumer.stats, moved_consumer.plugins)
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
	
	bool parse(RawPacketElem *raw_data, ParsedPacketElem *parsed_data){
		uint8_t *tcp_layer;
		iphdr *ip_layer = (iphdr*)(raw_data->data());
		uint8_t protocol;
		bool ret_val;
		
		switch (ip_layer->version){
			case IPV4_VER:
				tcp_layer = (uint8_t*)ip_layer + (ip_layer->ihl * 4);
				protocol = ip_layer->protocol;
				ret_val = plugins.ipv4(ip_layer, parsed_data);
				break;
			case IPV6_VER:
				tcp_layer = (uint8_t*)ip_layer + sizeof(ip6_hdr);
				protocol = ((ip6_hdr*)ip_layer)->ip6_ctlun.ip6_un1.ip6_un1_nxt;
				ret_val = plugins.ipv6((ip6_hdr*)ip_layer, parsed_data);
				break;
			default:
				tcp_layer = nullptr;
				protocol = 0;
				ret_val = false;
				break;
		}
		if (ret_val == false) return false;
		if (protocol == 0 || tcp_layer == nullptr) return true;
		
		uint8_t *app_layer;
		
		switch (protocol){
			case IPPROTO_TCP:
				app_layer = (uint8_t*)tcp_layer + (((tcphdr*)tcp_layer)->doff * 4);
				ret_val = plugins.tcp((tcphdr*)tcp_layer, parsed_data);
				break;
			case IPPROTO_UDP:
				app_layer = (uint8_t*)tcp_layer + sizeof(udphdr);
				ret_val = plugins.udp((udphdr*)tcp_layer, parsed_data);
				break;
			default: 
				app_layer = nullptr;
				ret_val = false;
				break;
		}
		if (ret_val == false) return false;
		if (app_layer == nullptr) return true;
		
		ret_val = plugins.app(app_layer, parsed_data);
		
		return ret_val;
	}	
	
	void run(){
		while (RUN_CONSUMER){
			RawPacketElem raw_data;
			ParsedPacketElem parsed_data;
			
			get(&raw_data);
			if (parse(&raw_data, &parsed_data)){
				stats->put(&(parsed_data.ip_layer.src_addr), &parsed_data);
			}
		}
	}
	
};

