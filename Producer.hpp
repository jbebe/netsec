#pragma once

#include <cstdlib>
#include <mutex>
#include <chrono>
#include <thread>


#include "Globals.hpp"
#include "Consumer.hpp"


class Producer {
	
private:
	// foreign data
	std::vector<Consumer> *consumers_ptr;
	
	// personal data
	
public:
	Producer(std::vector<Consumer> *consumers_ptr = nullptr)
	: consumers_ptr{consumers_ptr}
	{}
	
	int get(){
		int r = rand();
		dbg_printf("produced: %d\n", r);
		return r;
	}
	
	void run(){
		while (RUN){
			
			for (auto &consumer : *consumers_ptr){
				if (consumer.full() == false){
					consumer.lockBuffer();
					consumer.insertConsumable(get());
					consumer.unlockBuffer();
				}
			}
			
			// human readable
			std::this_thread::sleep_for(std::chrono::milliseconds(50));
		}
	}
	
};