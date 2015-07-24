#pragma once

#include <vector>
#include <mutex>

#include <netinet/in.h>

#include "../debug.hpp"
#include "ParsedPacketElem.hpp"
#include "IPv46.hpp"
#include "../Globals.hpp"

class Evaluator {
private:
	std::unordered_map<IPv46, std::vector<ParsedPacketElem>, IPv46::Hash> stats;
	std::mutex m;

public:

	void evaluate(std::vector<ParsedPacketElem> &data){
		/*
		 So here come the evaluator plugins
		*/
		dbg_printf("TTL: %d\n", data[0].ip_layer.ttl);
		data.clear();
	}
	
	void put(const IPv46 *ip, const ParsedPacketElem *elem) {
		std::lock_guard<std::mutex> lg{m};
		auto stat = stats.find(*ip);
		if (stat != stats.end()) {
			if (stat->second.size() < STATS_ENTRY_SIZE) {
				stat->second.push_back(*elem);
			}
			else {
				evaluate(stat->second);
			}
		} else {
			stats.emplace(*ip, std::vector<ParsedPacketElem>{});
		}
	}

};