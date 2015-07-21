#pragma once

#include <mutex>

#include <netinet/in.h>

#include "ParsedPacketElem.hpp"
#include "IPv46.hpp"

class StatsBuffer {

private:
	std::unordered_map<IPv46, std::vector<ParsedPacketElem>, IPv46::Hash> stats_data;
	std::mutex m;
	
public:
	StatsBuffer(){}
	
	void put(const IPv46 *ip, const ParsedPacketElem *elem){
		m.lock();
		auto stat = stats_data.end();//stats_data.find(*ip);
		if (stat != stats_data.end()){
			stat->second.push_back(*elem);
		}
		else {
			stats_data.emplace(*ip, std::vector<ParsedPacketElem>{});
		}
	}
	
	void get(){
		
	}
};