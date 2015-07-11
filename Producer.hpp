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
		
		auto t_start = std::chrono::high_resolution_clock::now();
		{
			auto data = get();
			while (1){
				for (auto &consumer : *consumers){
					// sync consumer.put(get());
					// async-ish:
					if (consumer.try_put(data)){
						data = get();
					}
				}
				if (performance_counter > 2000000UL) break;
			}
		}
		auto t_end = std::chrono::high_resolution_clock::now();
		dbg_printf("time: %lf ms\n", std::chrono::duration<double, std::milli>(t_end-t_start).count());

	}
	
};