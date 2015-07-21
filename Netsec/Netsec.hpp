#pragma once

#include <atomic>
#include <vector>
#include <initializer_list>
#include <thread>
#include <unordered_map>

#include <netinet/in.h>

#include "../Signals.hpp"
#include "PacketProducer.hpp"
#include "RawPacketElem.hpp"
#include "PacketConsumer.hpp"
#include "ConsumerPlugins.hpp"
#include "ParsedPacketElem.hpp"
#include "StatsBuffer.hpp"

class Netsec { 

	using PacketConsumerClass = PacketConsumer<8 /* buffer size */>;
	using PacketProducerClass = PacketProducer<PacketConsumerClass>;
	
	// raw packet level
	PacketProducerClass producer;
	std::vector<PacketConsumerClass> consumers;
	std::vector<std::thread> threads;
	
	// main (stats) buffer
	StatsBuffer stats_data;
	
public:
	Netsec(const char *interface): producer{interface} {
		// init signals
		init_signals();
		
		// get core num
		int cores = std::max<int>(std::thread::hardware_concurrency(), 1);

		// num. of consumers equals cores minus producer thread
		int consumers_num = std::max(cores - 1, 1);
		consumers.reserve(consumers_num);
		for (int i = 0; i < consumers_num; i++){
			// ctor with every layer of plugins
			consumers.emplace_back(plugin_IPv4, plugin_IPv6, plugin_TCP, plugin_UDP, plugin_APP);
		}

		// number of producers equals number of interfaces
		//producer = PacketProducerClass(interface);

		threads.reserve(cores);
	}
	
	void start(){
		// start flag
		// TODO
		
		// start producer(s)
		threads.emplace_back(&PacketProducerClass::run, &producer, &consumers);
		
		// start consumers
		for (auto &consumer : consumers){
			threads.emplace_back(&PacketConsumerClass::run, &consumer);
		}
		
		// wait for end
		for (auto &thread : threads){
			thread.join();
		}
	}

};