#pragma once

#include <vector>
#include <cstdint>
#include <mutex>
#include <thread>
#include <atomic>
#include <condition_variable>

#include "Config.hpp"
#include "Producer.hpp"
#include "Consumer.hpp"
#include "_debug.hpp"

namespace NETSEC {

class RingBuffer;

class RingBuffer {

public:	
	class BufferEntry {
	
	private:
		RingBuffer *buffer;
		uint32_t data;
	
	public:
		std::atomic<bool> locked;
	
		BufferEntry() = delete;
		
		BufferEntry(RingBuffer *parent_buffer, uint32_t input_data)
		: buffer{parent_buffer}, data{input_data} 
		{}
		
		inline uint32_t getData(){
			uint32_t tmp_data = data;
			// data consumed -> consumable decreased
			buffer->consumable -= 1;
			// chance to produce -> wake up a sleeping producer
			// this does nothing if there is no waiting thread
			buffer->cond_produce.notify_one();
			return tmp_data;
		}
		
		inline void setData(uint32_t new_data){
			dbg_printf("data set\n");
			data = new_data;
			// data arrived -> consumable increased
			buffer->consumable += 1;
			// chance to consume -> wake up sleeping consumer
			buffer->cond_consume.notify_one();
		}
	};
	
	template <int Ttrigger>
	class iterator {
	
	private:
		RingBuffer *ringbuff;
		int pos;
		std::condition_variable *cond_var;
	
	public:
		iterator(RingBuffer *rb, std::condition_variable *cv)
		: ringbuff{rb}, pos{0}, cond_var{cv} 
		{}
		
		//iterator &operator++(int) = delete;
		
		iterator &operator++(){
			
			// if there is no stuff to consume -> Ttrigger = 0
			// if there is no space to produce -> Ttrigger = CYCBUFF_CAPACITY
			if (ringbuff->consumable == Ttrigger){
				std::unique_lock<std::mutex> uniq_lck{ringbuff->mutex};
				cond_var->wait(uniq_lck);
			}
			
			pos = (pos + 1) % BUFF_CAPACITY;
			
			dbg_printf("iterator increased (%d)\n", (uint16_t)ringbuff->consumable);
			
			return *this;
		}
		
		BufferEntry &operator*(){
			return (ringbuff->data)[pos];
		}
		
		BufferEntry *operator->(){
			return &(ringbuff->data[pos]);
		}
		
	};
	
private:
	// thread management
	std::vector<std::thread> threads;
	std::mutex mutex;
	std::condition_variable cond_consume, cond_produce;
	
	// internal vars
	BufferEntry *data;
	std::atomic<uint16_t> consumable;
	
	// containers
	std::vector<std::unique_ptr<Producer>> producers;
	std::vector<std::unique_ptr<Consumer>> consumers;

public:

	RingBuffer()
	: data{static_cast<BufferEntry *>(
		::operator new(sizeof(BufferEntry) * BUFF_CAPACITY))} 
	{
		for (int i = 0; i < BUFF_CAPACITY; i++){
			void *entry = static_cast<void *>(&(data[i]));
			new (entry) BufferEntry{this, 0};
		}
	}
	
	void addProducer(Producer *p) {
		producers.emplace_back(p);
	}

	void addConsumer(Consumer *c) {
		consumers.emplace_back(c);
	}
	
	void start(){
		
		// start producer jobs
		for (auto &producer_uniq_ptr : producers){
			threads.emplace_back(&Producer::job<iterator<BUFF_CAPACITY>>, &(*producer_uniq_ptr), iterator<BUFF_CAPACITY>{this, &cond_consume});
		}
		
		// start consumer jobs
		for (auto &consumer_uniq_ptr : consumers){
			//threads.emplace_back(&Consumer::job<iterator<0>>, &(*consumer_uniq_ptr), iterator<0>{this, &cond_produce});
		}
		
		// wait for threads to end
		for (auto &thread : threads){
			thread.join();
		}
		
		dbg_printf("consumer & producer threads ended\n");
	}
	
	void wakeThreads(){
		cond_consume.notify_all();
		cond_produce.notify_all();
	}
	/*
	~Buffer();
	*/
	
}; // Buffer

} // NETSEC NS 
