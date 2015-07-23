#pragma once

#include <mutex>

#include <netinet/in.h>

#include "ParsedPacketElem.hpp"
#include "IPv46.hpp"
#include "../Globals.hpp"

class StatsBuffer {

private:
	std::unordered_map<IPv46, std::vector<ParsedPacketElem>, IPv46::Hash> stats_data;
	std::mutex m;
	
public:
	void put(const IPv46 *ip, const ParsedPacketElem *elem){
		std::lock_guard<std::mutex> lg{m};
		auto stat = stats_data.find(*ip);
		if (stat != stats_data.end()){
			stat->second.push_back(*elem);
		}
		else {
			std::vector<ParsedPacketElem> tmp;
			tmp.reserve(STATS_BUFFER_ENTRY_SIZE);
			stats_data.emplace(*ip, tmp);
		}
	}
	
	void get(const IPv46 *ip, ParsedPacketElem *elem){
		std::lock_guard<std::mutex> lg{m};
		
		
	}
};