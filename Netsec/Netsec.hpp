#pragma once

#include <atomic>
#include <vector>
#include <initializer_list>
#include <thread>

#include "../Signals.hpp"
#include "../CoPro/Consumer.hpp"
#include "../CoPro/Producer.hpp"
#include "PacketProducer.hpp"
#include "PacketElem.hpp"

class Netsec { 

	static constexpr int BUFFER_SIZE = 8;
	using elem_type = PacketElem;
	using default_consumer = Consumer<elem_type, BUFFER_SIZE>;
	using PacketProducerClass = PacketProducer<default_consumer>;
	
	// raw packet level
	PacketProducerClass producer;
	std::vector<default_consumer> consumers;
	std::vector<std::thread> threads;
	
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
			consumers.emplace_back();
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
			threads.emplace_back(&default_consumer::run, &consumer);
		}
		
		// wait for end
		for (auto &thread : threads){
			thread.join();
		}
	}

};