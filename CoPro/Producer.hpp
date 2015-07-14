#pragma once

#include <cstdlib>
#include <mutex>
#include <condition_variable>
#include <chrono>
#include <thread>
#include <sstream>

#include "../Globals.hpp"
#include "Consumer.hpp"

template <typename Tconsumer, typename Telem>
class Producer {

protected:
	std::vector<Tconsumer> *consumers;
	
public:
	Producer(std::vector<Tconsumer> *consumers_ptr = nullptr)
	: consumers{consumers_ptr} 
	{}
	
	Producer(const Producer &) = delete;
	
	Telem get(){
		return rand();
	}

	void run(){
		//auto t_start = std::chrono::high_resolution_clock::now();
		{
			Telem data = get();
			while (1){
				for (auto &consumer : *consumers){
					if (consumer.try_put(data)){
						data = get();
					}
				}
			}
		}
		//auto t_end = std::chrono::high_resolution_clock::now();
		//dbg_printf("time: %lf ms\n", std::chrono::duration<double, std::milli>(t_end-t_start).count());
	}
};