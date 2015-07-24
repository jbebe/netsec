#pragma once

#include <mutex>

#include <netinet/in.h>

#include "ParsedPacketElem.hpp"
#include "IPv46.hpp"
#include "../Globals.hpp"

class Evaluator {
private:
	std::unordered_map<IPv46, std::vector<ParsedPacketElem>, IPv46::Hash> stats;
	std::mutex m;

public:

	void put(const IPv46 *ip, const ParsedPacketElem *elem) {
		std::lock_guard<std::mutex> lg{m};
		auto stat = stats.find(*ip);
		if (stat != stats.end()) {
			if (stat->second.size() < STATS_ENTRY_SIZE) {
				stat->second.push_back(*elem);
			}
		} else {
			stats.emplace(*ip, std::vector<ParsedPacketElem>{});
		}
	}

	void evaluate() {
		std::lock_guard<std::mutex> lg{m};
		for (auto &stats_it : stats) {
			if (stats_it.second.size() == STATS_ENTRY_SIZE) {
				/* 
				basic evaluator plugin:
				if ttl is not an OS specific constant, then we're suspicious
				*/
				for (auto &vec_it: stats_it.second){
					if (vec_it.ip_layer.ttl == 63){
						// TODO: scope too deep
						dbg_printf("ttl: 63!\n");
					}
				}
			}
		}
	}

};