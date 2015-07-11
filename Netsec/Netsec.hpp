#pragma once

#include <atomic>
#include <vector>
#include <initializer_list>
#include <thread>

#include "Globals.hpp"
#include "Signals.hpp"
#include "debug.hpp"
#include "CoProWrapper.hpp"

class Netsec { 

	CoProWrapper<> cpw;
	
public:
	Netsec(const char *interface){
		// init signals
		init_signals();
	}
	
	void start(){
		cpw.start();
	}

};