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
	
	/*void dbg_drawBuffer(const char *str = ""){
		std::stringstream sb;
		sb << "[core " << id << "] " << str << " [";
		for (int i = 0; i <= BUFF_MAXINDEX; i++){
			if (i <= filled_pos)
				sb << "x";
			else 
				sb << ".";
		}
		dbg_printf("%s] fill_pos: %d\n", sb.str().c_str(), filled_pos);
	}*/

	void put(int data){
		queue.put(data);
	}
	
	int get(){
		return queue.get();
	}
	
	void run(){
		while (RUN){
			int data = get();
			dbg_printf(".%n", &data);
		}
	}
	
};
