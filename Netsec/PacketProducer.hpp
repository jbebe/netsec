#pragma once

#include "../CoPro/Consumer.hpp"
#include "../CoPro/Producer.hpp"
#include "PacketElem.hpp"

typedef Consumer<int, 8> default_consumer;
typedef Producer<default_consumer, int> default_producer;

class PacketProducer : public default_producer {
		
	int packet;
	
public:
	using default_producer::Producer;
	
	int *get(){
		packet = 5;
		return &packet;
	}
	
	void run(){
		int *data = get();
		while (1){
			for (auto &consumer : *consumers){
				// sync consumer.put(get());
				// async-ish:
				if (consumer.try_put(data)){
					data = get();
				}
			}
		}
	}
	
};