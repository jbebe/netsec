#pragma once

#include <vector>
#include <mutex>
#include <initializer_list>

#include <netinet/in.h>

#include "../debug.hpp"
#include "ParsedPacketElem.hpp"
#include "IPv46.hpp"
#include "../Globals.hpp"
#include "EvaluatorInfo.hpp"

class Evaluator {

public:
	using plugin_function = std::function<void(std::vector<ParsedPacketElem>&, EvaluatorInfo&)>;
	using plugin_pair = std::pair<EvaluatorInfo, plugin_function>;
	
private:
	std::unordered_map<IPv46, std::vector<ParsedPacketElem>, IPv46::Hash> stats;
	std::mutex m;

	std::vector<plugin_pair> plugins;

public:
	
	Evaluator() = delete;
	
	Evaluator(std::initializer_list<plugin_pair> plugins_il) {
		for (auto &plugin : plugins_il){
			plugins.emplace_back(plugin);
		}
	}

	void evaluate(std::vector<ParsedPacketElem> &ip_data){
		for (auto &plugin : plugins){
			plugin.second(ip_data, plugin.first);
		}
		
		/* TODO: MODULES */
		for (auto &plugin : plugins){
			std::tm *ptm = std::localtime(&plugin.first.timestamp);
			char buffer[32];
			std::strftime(buffer, 32, "%d.%m.%Y %H:%M:%S", ptm); // Format: Mo, 15.06.2009 20:20:00
			dbg_printf("time: %s | type: %s | prob: %f | info: %s\n", buffer, plugin.first.type.c_str(), plugin.first.probability, plugin.first.info.c_str());
		}
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
				stat->second.clear();
			}
		} else {
			stats.emplace(*ip, std::vector<ParsedPacketElem>{});
		}
	}

};
