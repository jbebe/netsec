#pragma once

#include <atomic>
#include <mutex>
#include <condition_variable>
#include <chrono>
#include <thread>
#include <type_traits>

#include "debug.hpp"
#include "Globals.hpp"

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 

class BufferEntry {

public:
	int data;
	bool consumed;
	BufferEntry(): data{0}, consumed{true} {}
	
};

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 

class Consumer {
	static int counter;
	
public:
	static constexpr int BUFF_SIZE = 8;
	
public:
	// personal data
	BufferEntry buffer[BUFF_SIZE];
	BufferEntry *buffer_end;
	std::atomic<unsigned short> consumable;
	std::mutex buffer_mut;
	
	std::mutex cond_mut;
	std::unique_lock<std::mutex> cond_lock;
	std::condition_variable cond;
	
	// foreign data
	std::condition_variable *producer_cond;
	
	// debug
	int id;
	
public:
	Consumer()
	: buffer_end{&buffer[BUFF_SIZE]}, consumable{0}, cond_lock{cond_mut}, producer_cond{nullptr}, id{counter++}
	{}
	
	Consumer(Consumer &&moved_consumer)
	: Consumer() 
	{}
	
	Consumer(Consumer &) = delete;
	
	void setProducerCondition(std::condition_variable *pc){
		producer_cond = pc;
	}
	
	// TODO: store last 
	void putConsumable(int data){
		for (BufferEntry *ptr = buffer; ptr != buffer_end; ++ptr){
			if (ptr->consumed == true){
				ptr->consumed = false;
				ptr->data = data;
				++consumable;
				return;
			}
		}
	}
	
	int getConsumable(){
		for (BufferEntry *ptr = buffer; ptr != buffer_end; ++ptr){
			if (ptr->consumed == false){
				ptr->consumed = true;
				--consumable;
				return ptr->data;
			}
		}
		return -1;
	}
	
	void dbg_drawBuffer(){
		std::stringstream sb;
		sb << "core " << id << " [";
		for (BufferEntry *ptr = buffer; ptr != buffer_end; ptr++){
			if (ptr->consumed)
				sb << ".";
			else 
				sb << "x";
		}
		dbg_printf("%s] %d\n", sb.str().c_str(), (int)consumable);
	}
	
	bool full() const {
		return (unsigned short)consumable == BUFF_SIZE;
	}
	
	bool empty() const {
		return (unsigned short)consumable == 0;
	}
	
	inline void lockBuffer(){
		buffer_mut.lock();
	}
	
	inline void unlockBuffer(){
		buffer_mut.unlock();
	}
	
	void notify(){
		cond.notify_one();
	}
	
	void run(){
		
		while (RUN){
			
			if (empty() == false){
				
				lockBuffer();
				dbg_drawBuffer();
				int data = getConsumable();
				dbg_printf("%n", &data);
				dbg_drawBuffer();
				unlockBuffer();

				if (consumable == BUFF_SIZE - 1)
					producer_cond->notify_one();

				++performance_counter;

				// human readable
				//std::this_thread::sleep_for(std::chrono::milliseconds(1000));
			}
			else {
				dbg_printf("core %d wait\n", id);
				// wait for data because it's empty
				cond.wait(cond_lock/*, [this]{ return !this->empty(); }*/);
				dbg_printf("core %d notified\n", id);
			}
		}
		
		// warn everyone
		producer_cond->notify_one();
		
	}
	
};

int Consumer::counter = 0;
