#pragma once

#include <initializer_list>
#include <cstdint>

#include "Config.hpp"

#include "Buffer.hpp"
#include "PcapProducer.hpp"

namespace NETSEC {

class Netsec {

private:

	Buffer buffer;
	
public:
	
	Netsec(std::initializer_list<const int8_t *> interface_list){
		
		// init producers
		for (const int8_t *interface_name : interface_list){
			buffer.addProducer(new PcapProducer(interface_name));
		}
		
		// init consumers
		for (uint32_t i = 0; i < std::max(NETSEC::CORE_COUNT - interface_list.size(), 1); i++){
			buffer.addConsumer(new DummyConsumer());
		}
		
	}
	
	void start(){
		
		// start getting packets
		buffer.start();
		
	}
	
}; // Netsec

} // NETSEC NS