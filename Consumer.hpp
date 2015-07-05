#pragma once

#include <atomic>
#include <mutex>

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
	std::atomic<unsigned short> consumed;
	std::mutex mutex; 
	std::unique_lock<std::mutex> lock;
	
public:
	Consumer()
	: consumed{BUFF_SIZE}, buffer_end{&buffer[BUFF_SIZE]}, mutex{}, lock{mutex}
	{}
	
	Consumer(Consumer &&moved_consumer)
	: Consumer() 
	{}
	
	void put(int data){
		BufferEntry *ptr = buffer;
		while (ptr != buffer_end){
			if (ptr->consumed == true){
				ptr->data = data;
				return;
			}
		}
	}
	
	bool full(){
		return consumed == 0;
	}
	
	void lockBuffer(){
		lock.lock();
	}
	
	void unlockBuffer(){
		lock.unlock();
	}
	
	void run(){
		
	}
	
};