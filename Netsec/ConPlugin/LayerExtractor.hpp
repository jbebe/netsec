#pragma once

#include <cstdint>
#include <netinet/ip.h>
#include <netinet/ip6.h>
#include <netinet/tcp.h>
#include <netinet/udp.h>

#include "../ParsedPacketElem.hpp"

static constexpr bool OK = true;
static constexpr bool ABORT = false;

bool plugin_IPv4(iphdr *data, ParsedPacketElem *elem){
	elem->ip_layer.ttl = data->ttl;
	elem->ip_layer.src_addr = IPv46::fromIPv4(data->saddr);
	
	return OK;
}

bool plugin_IPv6(ip6_hdr *data, ParsedPacketElem *elem){
	elem->ip_layer.ttl = data->ip6_ctlun.ip6_un1.ip6_un1_hlim;
	elem->ip_layer.src_addr = IPv46{data->ip6_src.__in6_u.__u6_addr32};
	
	return OK;
}

bool plugin_TCP(tcphdr *data, ParsedPacketElem *elem){
	elem->tcp_layer.dest_port = data->dest;
	elem->tcp_layer.src_sport = data->source;
	
	return OK;
}

bool plugin_UDP(udphdr *data, ParsedPacketElem *elem){
	elem->tcp_layer.dest_port = data->dest;
	elem->tcp_layer.src_sport = data->source;
	
	return OK;
}

bool plugin_APP(uint8_t *data, ParsedPacketElem *elem){
	return OK;
}