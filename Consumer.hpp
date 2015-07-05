#pragma once

#include <chrono>
#include <thread>

#include "Config.hpp"
#include "_debug.hpp"

namespace NETSEC {

class Consumer {
	
private:

public:
	template <typename Titerator>
	void job(Titerator &&it) {
		while (STOP == false){
			if (it->locked == false){
				it->locked = true;
				// get data from buffer
				auto data = it->getData();
				it->locked = false;
				dbg_printf("%consumed: %d\n", data);
			}
			++it;
			// debug retard
			std::this_thread::sleep_for(std::chrono::milliseconds(500));
		}
	}
	
};

}