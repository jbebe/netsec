#pragma once

#include <cassert>
#include <atomic>
#include <mutex>
#include <condition_variable>
#include <chrono>
#include <thread>
#include <type_traits>

#include "debug.hpp"
#include "Globals.hpp"
#include "MTStack.hpp"

template <typename Telem, int Tcapacity>
class Consumer {

	MTStack<Telem, Tcapacity> queue;
	std::mutex cond_mut;
	std::condition_variable cond;
	
public:
	Consumer(){}
	
	Consumer(Consumer &&moved_consumer): Consumer() {}
	
	Consumer(const Consumer &) = delete;

	void put(Telem data){
		queue.put(data);
	}
	
	bool try_put(Telem data){
		return queue.try_put(data);
	}
	
	Telem get(){
		return queue.get();
	}
	
	void run(){
		while (1){
			Telem data = get();
			dbg_printf("%d\n", data);
		}
	}
	
};
