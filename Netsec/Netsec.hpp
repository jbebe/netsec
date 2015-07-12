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
	typedef int Elem;
	typedef Consumer<Elem, BUFFER_SIZE> default_consumer;
	
	CoProWrapper<Elem, BUFFER_SIZE, default_consumer, PacketProducer> cpw;
	
public:
	Netsec(const char *interface){
		// init signals
		init_signals();
	}
	
	void start(){
		cpw.start();
	}

};