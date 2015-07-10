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

class Consumer {

	MTStack<> queue;
	std::mutex cond_mut;
	std::condition_variable cond;
	
public:
	Consumer(){}
	
	Consumer(Consumer &&moved_consumer): Consumer() {}
	
	Consumer(const Consumer &) = delete;

	void put(int data){
		queue.put(data);
	}
	
	bool try_put(int &data){
		return queue.try_put(data);
	}
	
	int get(){
		return queue.get();
	}
	
	void run(){
		while (1){
			int data = get();
			dbg_printf("%n", &data);
			++performance_counter;
		}
	}
	
};
