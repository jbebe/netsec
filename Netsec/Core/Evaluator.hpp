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
 The role of this class is to provide a central unit 
 for parsed data, plugins and modules
 */
class Evaluator {

public:
	// Type signature for evaluator plugin
	using plugin_fn_t = void(
		const std::vector<ParsedPacketElem>&, EvaluatorInfo&);
	/*
	 Container for parser plugins and their temporary store
	 Same as std::pair but with variable names that speak to theirselves
	 */
	struct PluginPair {
		std::function<plugin_fn_t> function;
		EvaluatorInfo data;
		PluginPair(
			std::function<plugin_fn_t> function = std::function<plugin_fn_t>{},
			EvaluatorInfo data = EvaluatorInfo{}
		): function{function}, data{data} {}
	};
	// Type signature for evaluator module
	using module_fn_t = void(
		const std::vector<ParsedPacketElem>&, const std::vector<PluginPair>&);
	using module_t = std::function<module_fn_t>;
	
private:
	/*
	 hashmap to store 'ip' -> 'packet data collection' structure
	 the collection has to be big enough to do statistical analysis
	 */
	std::unordered_map<IPv46, std::vector<ParsedPacketElem>, IPv46::Hash> stats;
	/*
	 this collection stores plugin functions plus their own informations
	 this way we can maintain the outcoming informations in a unified struct 
	 */ 
	std::vector<PluginPair> plugins;
	// modules mostly for output
	std::vector<module_t> modules;
	// mutex to lock the whole class while working on it
	// TODO: THIS IS WRONG! 
	// but we can't use multiple hashmaps because 
	// we want one warning for a single detection
	// so we have to optimize when to lock and unlock
	// IDEA: multiple mutexes for different data
	std::mutex m;

public:
	
	Evaluator() = delete;
	
	Evaluator(std::initializer_list<PluginPair> plugins_il, std::initializer_list<module_t> modules_il) {
		for (auto &plugin : plugins_il){
			plugins.emplace_back(plugin);
		}
		for (auto &module : modules_il){
			modules.emplace_back(module);
		}
	}

	void evaluate(const std::vector<ParsedPacketElem> &ip_data){
		for (auto &plugin : plugins){
			plugin.function(ip_data, plugin.data);
		}
		for (auto &module : modules){
			module(ip_data, plugins);
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