#pragma once

#include <cstdint>
#include <sstream>

#include "../../debug.hpp"
#include "../EvaluatorInfo.hpp"
#include "../ParsedPacketElem.hpp"

/*
 A basic detection
 */
class NatDetectionPlugin {

private:
	std::unordered_map<uint8_t, std::string> ttl_os_list;
	std::vector<bool> ttl_valid_table;
	
public:
	NatDetectionPlugin(){
		ttl_os_list.emplace(std::make_pair(30, "DEC,HP-UX,AIX,Ultrix (TCP/UDP)"));
		ttl_os_list.emplace(std::make_pair(32, "Windows NT,95 (TCP/UDP)"));
		ttl_os_list.emplace(std::make_pair(60, "MacOS (TCP/UDP)"));
		ttl_os_list.emplace(std::make_pair(64, "FreeBSD,Linux,MacOS,OS/2 (ICMP/TCP/UDP)"));
		ttl_os_list.emplace(std::make_pair(128, "Windows XP,7,8,10 (ICMP/TCP/UDP)"));
		ttl_os_list.emplace(std::make_pair(255, "FreeBSD,Linux,NetBSD (ICMP)"));
		
		ttl_valid_table.resize(256, false);
		ttl_valid_table.at(30) = ttl_valid_table.at(32) = 
			ttl_valid_table.at(60) = ttl_valid_table.at(64) = 
			ttl_valid_table.at(128) = ttl_valid_table.at(255) = true;
	}
	
	void operator () (
		const std::vector<ParsedPacketElem> &packets, EvaluatorInfo &info){
		
		info.timestamp = std::time(0);
		info.type = "NAT";
		for (auto &packet : packets){
			auto ttl = packet.ip_layer.ttl;
			if (!ttl_valid_table[ttl]){
				int hops = 0;
				uint8_t i;
				for (i = ttl; !ttl_valid_table[i]; i++, hops++);
				char info_buffer[256];
				snprintf(info_buffer, 256, "TTL: %hhu (should be %hhu) OS: %s", ttl, i, ttl_os_list[i].c_str());
				info.info = info_buffer;
				info.probability = 1.0f;
				return;
			}
		}
		info.info = "-";
		info.probability = 0.0f;
	}

};
