#pragma once

#include <cstdint>
#include <netinet/ip.h>
#include <netinet/ip6.h>
#include <netinet/tcp.h>
#include <netinet/udp.h>

#include "ParsedPacketElem.hpp"

void plugin_IPv4(iphdr *data, ParsedPacketElem *elem){
	/*
	uint32_t ip_addr = ntohl(data->saddr);
	struct _ipa { uint8_t d, c, b, a; };
	_ipa *padded_ip = (_ipa*)(&ip_addr);
	dbg_printf("%d.%d.%d.%d\n", padded_ip->a, padded_ip->b, padded_ip->c, padded_ip->d);
	*/
	elem->valid = true;
	elem->ip_layer.ttl = data->ttl;
}

void plugin_IPv6(ip6_hdr *data, ParsedPacketElem *elem){

}

void plugin_TCP(tcphdr *data, ParsedPacketElem *elem){
	
}

void plugin_UDP(udphdr *data, ParsedPacketElem *elem){

}

void plugin_APP(uint8_t *data, ParsedPacketElem *elem){
	
}