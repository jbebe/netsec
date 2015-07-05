#pragma once

#include <atomic>
#include <mutex>
#include <chrono>
#include <thread>


#include "debug.hpp"

class BufferEntry {

public:
	int data;
	bool consumed;
	BufferEntry(): data{0}, consumed{true} {}
	
};

class Consumer {
	
public:
	static constexpr int BUFF_SIZE = 8;
	
protected:
	BufferEntry buffer[BUFF_SIZE];
	BufferEntry *buffer_end;
	std::atomic<unsigned short> consumable;
	std::mutex mutex; 
	
public:
	Consumer()
	: buffer_end{&buffer[BUFF_SIZE]}, consumable{0}, mutex{}
	{}
	
	Consumer(Consumer &&moved_consumer)
	: Consumer() 
	{}
	
	void insertConsumable(int data){
		BufferEntry *ptr = buffer;
		while (ptr != buffer_end){
			if (ptr->consumed == true){
				ptr->data = data;
				ptr->consumed = false;
				++consumable;
				return;
			}
		}
	}
	
	int getConsumable(){
		BufferEntry *ptr = buffer;
		while (ptr != buffer_end){
			if (ptr->consumed == false){
				ptr->consumed = true;
				--consumable;
				return ptr->data;
			}
		}
		return -1;
	}
	
	inline bool full() const {
		return consumable == BUFF_SIZE;
	}
	
	inline bool empty() const {
		return consumable == 0;
	}
	
	inline void lockBuffer(){
		mutex.lock();
	}
	
	inline void unlockBuffer(){
		mutex.unlock();
	}
	
	void run(){
		
		while (RUN){
			if (empty() == false){
				lockBuffer();
				int data = getConsumable();
				unlockBuffer();
				dbg_printf("consumed: %d\n", data);
				
				// human readable
				std::this_thread::sleep_for(std::chrono::milliseconds(1000));
			}
		}
		
	}
	
};