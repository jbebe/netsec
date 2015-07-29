#pragma once

#include <vector>
#include <mutex>
#include <initializer_list>
#include <sstream>

#include <netinet/in.h>

#include "../../debug.hpp"
#include "../ParsedPacketElem.hpp"
#include "../IPv46.hpp"
#include "../../Globals.hpp"
#include "../EvaluatorInfo.hpp"

/*
 Evaluator class
 */
class Evaluator {

public:
	using plugin_function = std::function<void(const std::vector<ParsedPacketElem>&, EvaluatorInfo&)>;
	using plugin_pair = std::pair<EvaluatorInfo, plugin_function>;
	
	using module_function = std::function<void(const std::vector<plugin_pair>&)>;
	
private:
	std::unordered_map<IPv46, std::vector<ParsedPacketElem>, IPv46::Hash> stats;
	std::vector<plugin_pair> plugins;
	std::vector<module_function> modules;
	std::mutex m;

public:
	
	Evaluator() = delete;
	
	Evaluator(std::initializer_list<plugin_pair> plugins_il, std::initializer_list<module_function> modules_il) {
		for (auto &plugin : plugins_il){
			plugins.emplace_back(plugin);
		}
		for (auto &module : modules_il){
			modules.emplace_back(module);
		}
	}

	void evaluate(const std::vector<ParsedPacketElem> &ip_data){
		for (auto &plugin : plugins){
			plugin.second(ip_data, plugin.first);
		}
		/* TODO: MODULES */
		for (auto &module : modules){
			module(plugins);
		}
	}
	
	void put(const IPv46 * const ip, const ParsedPacketElem *elem) {
		std::lock_guard<std::mutex> lg{m};
		auto stat = stats.find(*ip);
		if (stat != stats.end()) {
			if (stat->second.size() < STATS_ENTRY_SIZE) {
				stat->second.push_back(*elem);
				
				/* - - - DEBUG - - - */
				{
					std::stringstream ss;
					ss << "Hasmap load / Informations\n";
					for (auto &dbg_stat : stats){
						char buff[256];
						snprintf(buff, 255, "%25s - %3zu/%3zu\n", 
							dbg_stat.first.str().c_str(),
							dbg_stat.second.size(),
							STATS_ENTRY_SIZE
						);
						ss << buff;
					}
					//dbg_printf("\033[H\033[J%s", ss.str().c_str());
					dbg_printf("\033[H%s", ss.str().c_str());
				}
				/* - - - DEBUG - - - */
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