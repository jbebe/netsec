#pragma once

#include <mutex>
#include <condition_variable>
#include <chrono>
#include <cstdint>

#include "../debug.hpp"
#include "../Globals.hpp"
#include "../CoPro/MTStack.hpp"

template <typename Telem, int Tcapacity>
class PacketConsumer {

	// consumer properties
	MTStack<Telem, Tcapacity> queue;
	std::mutex cond_mut;
	std::condition_variable cond;
	
	// plugins
	/*
	 a consumer kiveszi a raw packetet a run() függvényben
	 ip/tcp/stb. rétegeken kéne átvezetni a packetet
	 hogy mindenki kivehesse belőle azt amit akar és egy közös 
	 struct-ba tegye.
	 */
	
public:
	PacketConsumer(){}
	
	PacketConsumer(PacketConsumer &&moved_consumer): PacketConsumer() {}
	
	PacketConsumer(const PacketConsumer &) = delete;

	void put(Telem data){
		queue.put(data);
	}
	
	bool try_put(Telem *data){
		return queue.try_put(data);
	}
	
	void get(Telem *data_in){
		queue.get(data_in);
	}
	
	void run(){
		while (1){
			Telem data;
			get(&data);
			std::string log{data};
			dbg_printf("%s packet.\n", log.c_str());
		}
	}
	
	void pluginIPv4(uint8_t *data){
		
	}
	
	void pluginIPv6(uint8_t *data){
		
	}
	
};
