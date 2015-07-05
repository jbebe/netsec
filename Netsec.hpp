#pragma once

#include <initializer_list>
#include <iostream>

#include "Config.hpp"
#include "RingBuffer.hpp"
#include "Producer.hpp"
#include "Consumer.hpp"
#include "Signals.hpp"

namespace NETSEC {

class Netsec {

private:
	RingBuffer buffer;
	
public:
	Netsec(std::initializer_list<const char *> interface_list){
		
		// init signals
		init_signals();
		
		// init producers
		for (auto &interface_name : interface_list){
			buffer.addProducer(new Producer());
		}
		
		// init consumers
		for (int i = 0; i < std::max(CORE_COUNT - (int)interface_list.size(), 1); i++){
			buffer.addConsumer(new Consumer());
		}
		
	}	
	
	void start(){
		buffer.start();
	}

}; // Netsec

} // NETSEC NS