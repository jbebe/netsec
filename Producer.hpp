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
	
public:
	static std::atomic<bool> RUN;
	
private:
	// foreign data
	std::vector<Consumer> *consumers;
	
	// private data
	std::mutex cv_m;
	std::condition_variable cv;
	
public:
	Producer(std::vector<Consumer> *consumers_ptr = nullptr)
	: consumers{consumers_ptr} 
	{
		if (consumers_ptr != nullptr){
			for (auto &consumer : *consumers){
				consumer.setProducerCV(&cv);
			}
		}
	}
	
	Producer(const Producer &) = delete;
	
	// here comes the functors
	int get(){
		return rand();
	}

	void run(){
		
		while (RUN){
			bool all_full = true;
		
			for (auto &consumer : *consumers){
				bool is_full;
				if ((is_full = consumer.full()) == false){
					consumer.put(get());
				}
				all_full = all_full && is_full;
			}
			
			if (all_full){
				dbg_printf("producer wait\n");
				{
					std::unique_lock<std::mutex> ul{cv_m};
					cv.wait(ul);
				}
				dbg_printf("producer resumed\n");
			}
			
		}
		
		// warn neighboor
		for (auto &consumer : *consumers){
			consumer.notify();
		}
		std::this_thread::sleep_for(std::chrono::milliseconds(100));
		
	}
	
};

std::atomic<bool> Producer::RUN{true};