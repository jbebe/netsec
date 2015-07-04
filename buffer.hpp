#pragma once

#include <vector>
#include <cstdint>
#include <mutex>
#include <thread>
#include <atomic>
#include <condition_variable>

#include "Netsec.hpp"
#include "IProducer.hpp"
#include "IConsumer.hpp"
#include "IBufferJob.hpp"

namespace NETSEC {

class Buffer {

	struct BufferEntry {
		// [consumed: 1 byte][size: 2 bytes][data_addr: 4 bytes] -> 8 bytes
		// [consumed: 1 byte][data_addr: 4 bytes][size: 2 bytes] -> 12 bytes
		uint8_t consumed;
		uint16_t size;
		uint8_t *data_addr;
	};
	
private:
	// thread management
	std::mutex mutex;
	std::condition_variable cond_consume, cond_produce;
	std::vector<std::thread> threads;
	std::atomic<bool> stop_flag;
	
	// internal vars
	const std::vector<BufferEntry> buffer;
	
	// containers
	const std::vector<std::unique_ptr<IProducer>> producers;
	const std::vector<std::unique_ptr<IConsumer>> consumers;

public:

	Buffer()
	: stop_flag{false}
	{
		buffer.reserve(CYCBUFF_CAPACITY);
	}

	void addProducer(IProducer *p) {
		producers.emplace_back(p);
	}

	void addConsumer(IConsumer *c) {
		consumers.emplace_back(c);
	}
	
	void start(){
		
		// start producer jobs
		for (auto &producer_ptr : producers){
			threads.push_back(std::thread(producer_ptr->job, mutex, cond_produce, stop_flag, buffer));
		}
		
		for (auto &consumer_ptr : consumers){
			threads.push_back(std::thread(consumer_ptr->job, mutex, cond_consume, stop_flag, buffer));
		}
		
	}
	
	void stop(){
		stop_flag = true;
		cond_produce.notify_all();
		cond_consume.notify_all();
	}
	
	~Buffer(){
		for (auto& thread: threads) {
			thread.join();
		}
		// delete packets
	}

}; // Buffer

} // NETSEC NS