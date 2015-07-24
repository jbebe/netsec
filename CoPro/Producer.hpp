#pragma once

#include <vector>

#include "Consumer.hpp"

/*
 Producer class is the producer side 
 of a single-consumer - single producer data exchange structure 
 where consumer holds the data as a member variable.
 
 The current role of Producer class is to be an interface-dummy 
 to test the stability of MTStack.
 */

template <typename Tconsumer>
class Producer {

public:
	
	int get(){ return 5; }

	/*
	 run is used to be called as a thread 
	 so the cpu will handle the producer in a separate thread from other tasks
	 run gets the consumer's vector so it can iterate over it 
	 this way try_put makes the procedure way faster
	 */
	void run(std::vector<Tconsumer> *consumers){
		int data = get();
		while (1 /* TODO: exit flag and proper exit */){
			for (auto &consumer : *consumers){
				if (consumer.try_put(data)){
					data = get();
				}
			}
		}
	}
	
};