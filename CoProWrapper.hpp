#pragma once

#include <thread>
#include <vector>

#include "Consumer.hpp"
#include "Producer.hpp"

template <
	typename Telem = int, 
	int Tcapacity = 8,
	typename Tconsumer = Consumer<Telem, Tcapacity>, 
	typename Tproducer = Producer<Tconsumer, Telem>
>
class CoProWrapper {
	
	Tproducer producer;
	std::vector<Tconsumer> consumers;
	std::vector<std::thread> threads;
	
public:
	CoProWrapper(){

		int cores = std::max<int>(std::thread::hardware_concurrency(), 1);

		// num. of consumers equals cores minus producer thread
		consumers.reserve(cores - 1);
		for (int i = 0; i < std::max(cores - 1, 1); i++){
			consumers.emplace_back();
		}

		// number of producers equals number of interfaces
		new (static_cast<void*>(&producer)) Tproducer(&consumers);

		threads.reserve(cores);
	}
	
	void start(){
		
		// start flag
		//RUN = true;
		
		// start producer(s)
		threads.emplace_back(&Tproducer::run, &producer);
		
		// start consumers
		for (auto &consumer : consumers){
			threads.emplace_back(&Tconsumer::run, &consumer);
		}
		
		// wait for end
		for (auto &thread : threads){
			thread.join();
		}
	}
};