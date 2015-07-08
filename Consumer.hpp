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

class Consumer {

public:
	static std::atomic<bool> RUN;
	static int counter;
	static constexpr int BUFF_SIZE = 3;
	static constexpr int BUFF_MAXINDEX = (BUFF_SIZE - 1);
	static constexpr int BUFF_MININDEX = -1;
	
private:
	// personal data
	int buffer[BUFF_SIZE];
	std::mutex buffer_mut;
	
	std::mutex cond_mut;
	std::condition_variable cond;
	
	int filled_pos;
	
	// foreign data
	std::condition_variable *producer_cv;
	
	// debug
	int id;
	
public:
	Consumer()
	: filled_pos{-1}, id{counter++}
	{}
	
	Consumer(Consumer &&moved_consumer): Consumer() {}
	
	Consumer(const Consumer &) = delete;
	
	void setProducerCV(std::condition_variable *pcv){
		producer_cv = pcv;
	}
	
	void dbg_drawBuffer(const char *str = ""){
		std::stringstream sb;
		sb << "[core " << id << "] " << str << " [";
		for (int i = 0; i <= BUFF_MAXINDEX; i++){
			if (i <= filled_pos)
				sb << "x";
			else 
				sb << ".";
		}
		dbg_printf("%s] fill_pos: %d\n", sb.str().c_str(), filled_pos);
	}
	
	inline bool full() const {
		return filled_pos == BUFF_MAXINDEX;
	}
	
	inline bool empty() const {
		return filled_pos == BUFF_MININDEX;
	}
	
	void put(int data){
		bool must_notify = false;
		{
			std::lock_guard<std::mutex> lg{buffer_mut};
			//dbg_drawBuffer("put() before");
			buffer[++filled_pos] = data;
			dbg_drawBuffer("put() after");
			must_notify = (filled_pos == 0);
		}
		if (must_notify){
			cond.notify_one();
		}
	}
	
	int get(){
		{
			std::unique_lock<std::mutex> ul{cond_mut};
			if (empty()){
				dbg_printf("[core %d] get() wait\n", id);
				cond.wait(ul/*, [this]{ return !this->empty(); }*/);
				dbg_printf("[core %d] get() resumed\n", id);
			}
		}
		int data = -1;
		bool must_notify = false;
		{
			std::lock_guard<std::mutex> lg{buffer_mut};
			data = buffer[filled_pos--];
			must_notify = (filled_pos == (BUFF_MAXINDEX - 1));
		}
		if (must_notify){
			producer_cv->notify_one();
		}
		return data;
	}
	
	void notify(){
		cond.notify_one();
	}
	
	void run(){
		
		while (RUN){
			int data = get();
			dbg_printf("%n", &data);
			++performance_counter;
		}
		
		// warn neighboor
		producer_cv->notify_one();
		std::this_thread::sleep_for(std::chrono::milliseconds(100));
	}
	
};

int Consumer::counter = 0;

std::atomic<bool> Consumer::RUN{true};
