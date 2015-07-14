#pragma once

#include <atomic>
#include <vector>
#include <initializer_list>
#include <thread>

#include "../Globals.hpp"
#include "../Signals.hpp"
#include "../debug.hpp"
#include "../CoPro/CoProWrapper.hpp"
#include "PacketProducer.hpp"
#include "PacketElem.hpp"

class Netsec { 

	static constexpr int BUFFER_SIZE = 8;
	typedef PacketElem elem_type;
	typedef Consumer<elem_type, BUFFER_SIZE> default_consumer;
	typedef Producer<default_consumer> default_producer;
	
	CoProWrapper<elem_type, BUFFER_SIZE, default_consumer, PacketProducer<default_consumer>> cpw;
	
public:
	Netsec(const char *interface){
		// init signals
		init_signals();
		
	}
	
	void start(){
		cpw.start();
	}

};