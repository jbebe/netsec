#pragma once

#include <cstdlib>
#include <mutex>
#include <condition_variable>
#include <chrono>
#include <thread>


#include "Globals.hpp"
#include "Consumer.hpp"


class Producer {
	
private:
	// foreign data
	std::vector<Consumer> *consumers_ptr;
	
	// personal data
	std::mutex cond_mut;
	std::unique_lock<std::mutex> cond_lock;
	std::condition_variable cond;
	
public:
	Producer(std::vector<Consumer> *consumers_ptr = nullptr)
	: consumers_ptr{consumers_ptr}, cond_lock{cond_mut}
	{}
	
	int get(){
		int r = rand();
		return r;
	}
	
	void run(){
		while (RUN){
			
			bool all_full = true;
			
			for (auto &consumer : *consumers_ptr){
				
				bool is_full;
				
				if ((is_full = consumer.full()) == false){
					consumer.lockBuffer();
					consumer.putConsumable(get());
					consumer.unlockBuffer();
				}
				
				all_full &= is_full;
			}
			
			if (all_full){
				dbg_printf("producer in wait\n");
				cond.wait(cond_lock);
			}
			
			// human readable
			// producer works fast
			std::this_thread::sleep_for(std::chrono::milliseconds(50));
		}
	}
	
};