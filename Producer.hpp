#pragma once

#include <cstdlib>
#include <chrono>
#include <thread>

#include "Config.hpp"
#include "_debug.hpp"

namespace NETSEC {

class Producer {
	
private:

public:	
	template <typename Titerator>
	void job(Titerator &&it) {
		while (STOP == false){
			if (it->locked == false){
				it->locked = true;
				// put data into buffer
				it->setData(rand());
				it->locked = false;
			}
			++it;
			// debug retard
			std::this_thread::sleep_for(std::chrono::milliseconds(500));
		}
	}
	
};

}