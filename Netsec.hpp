#pragma once

#include <atomic>
#include <vector>
#include <initializer_list>
#include <thread>

#include "Producer.hpp"
#include "Consumer.hpp"
#include "Signals.hpp"
#include "debug.hpp"

class Netsec { 
	
private:
	Producer producer;
	std::vector<Consumer> consumers;
	std::vector<std::thread> threads;
	
public:
	Netsec(const char *interface){
		
		// init signals
		init_signals();
		
		int cores = std::max<int>(std::thread::hardware_concurrency(), 1);
		
		// number of consumers equals cores minus one producer/main thread
		consumers.reserve(cores - 1);
		for (int i = 0; i < cores - 1; i++){
			consumers.emplace_back();
		}
		
		// number of producers equals number of interfaces
		new (static_cast<void*>(&producer)) Producer(&consumers);
		
		threads.reserve(cores);
	}
	
	void start(){
		
		// start producer
		threads.emplace_back(&Producer::run, &producer);
		
		// start consumers
		for (auto &consumer : consumers){
			threads.emplace_back(&Consumer::run, &consumer);
		}
		
		// wait or end
		for (auto &thread : threads){
			thread.join();
		}
		
		dbg_printf("Exiting...");
	}

};