#pragma once

#include <atomic>
#include <vector>
#include <initializer_list>
#include <thread>
#include <unordered_map>

#include <netinet/in.h>

#include "../../Globals.hpp"
#include "../../Signals.hpp"
#include "PacketProducer.hpp"
#include "../RawPacketElem.hpp"
#include "PacketConsumer.hpp"
#include "../ParsedPacketElem.hpp"
#include "Evaluator.hpp"
#include "../ConPlugin/LayerExtractor.hpp"
#include "../EvalPlugin/NatDetect.hpp"
#include "../EvalModule/DummyModule.hpp"

class Netsec {
	
	// raw packet level
	PacketProducer producer;
	std::vector<PacketConsumer> consumers;
	std::vector<std::thread> threads;

	// main (stats) buffer + evaluator
	Evaluator stats_data;

public:
	Netsec(const char *interface) 
	: 
		// initialize producer with interface name
		producer{interface}, 
		// initialize Evaluator with plugins and modules
		stats_data{
			{ Evaluator::PluginPair{DummyFunctorPlugin{}, EvaluatorInfo{}} },
			{ dummy_module }
		}
	{
		// init signals
		init_signals();
		// num. of consumers equals cores minus producer thread
		int consumers_num = std::max<int>(CORE_NUM - 1, 1);
		consumers.reserve(consumers_num);
		for (int i = 0; i < consumers_num; i++) {
			// ctor with every layer of plugins
			consumers.emplace_back(
				&stats_data, PacketConsumer::PluginPack{
					plugin_IPv4, plugin_IPv6, plugin_TCP, plugin_UDP, plugin_APP
				}
			);
		}

		threads.reserve(CORE_NUM);
	}

	void start() {

		// start producer(s)
		threads.emplace_back(&PacketProducer::run, &producer, &consumers);

		// start consumers
		for (auto &consumer : consumers) {
			threads.emplace_back(&PacketConsumer::run, &consumer);
		}

		// wait for end
		for (auto &thread : threads) {
			thread.join();
		}
	}

};
