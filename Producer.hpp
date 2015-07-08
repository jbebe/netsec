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
	
private:
	// foreign data
	std::vector<Consumer> *consumers;
	
	// private data
	std::mutex cond_mut;
	std::unique_lock<std::mutex> cond_lock;
	std::condition_variable cond;
	
public:
	Producer()
	: consumers{nullptr}, cond_lock{cond_mut}
	{}
	
	Producer(std::vector<Consumer> *consumers_ptr)
	: consumers{consumers_ptr}, cond_lock{cond_mut}
	{
		for (auto &consumer : *consumers_ptr){
			consumer.setProducerCondition(&cond);
		}
	}
	
	Producer(Producer &) = delete;
	
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
					consumer.lockBuffer();
					consumer.putConsumable(get());
					consumer.unlockBuffer();
					if (consumer.consumable == 1)
						consumer.notify();
				}
				
				all_full &= is_full;
			}
			
			if (all_full){
				dbg_printf("producer wait\n");
				cond.wait(cond_lock);
				dbg_printf("producer notified\n");
			}
			/*else{
				// human readable
				// producer works fast
				std::this_thread::sleep_for(std::chrono::milliseconds(1));
			}*/
		}
		
		// warn everyone that it's over
		for (auto &consumer : *consumers){
			consumer.notify();
		}
		
	}
	
};