#pragma once

#include <cstdlib>
#include <mutex>
#include <condition_variable>
#include <chrono>
#include <thread>
#include <sstream>

#include "Globals.hpp"
#include "Consumer.hpp"

class Producer {

	// foreign data
	std::vector<Consumer> *consumers;
	
	// private data
	std::mutex cv_m;
	std::condition_variable cv;
	
public:
	Producer(std::vector<Consumer> *consumers_ptr = nullptr)
	: consumers{consumers_ptr} 
	{}
	
	Producer(const Producer &) = delete;
	
	int get(){
		return rand();
	}

	void run(){
		while (RUN){
			for (auto &consumer : *consumers){
				consumer.put(get());
			}
		}
	}
	
};