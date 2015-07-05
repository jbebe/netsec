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
		return rand();
	}
	
	void run(){
		int rotator = 0;
		while (RUN){
			
			for (auto &consumer : *consumers_ptr){
				if (consumer.full() == false){
					consumer.lockBuffer();
					consumer.put(get());
					consumer.unlockBuffer();
				}
			}
			
			// human readable
			std::this_thread::sleep_for(std::chrono::milliseconds(500));
		}
	}
	
};