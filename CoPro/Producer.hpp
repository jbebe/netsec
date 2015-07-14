#pragma once

#include <vector>

#include "Consumer.hpp"

template <typename Tconsumer>
class Producer {

public:
	int get(){
		return rand();
	}

	void run(std::vector<Tconsumer> *consumers){
		int data = get();
		while (1){
			for (auto &consumer : *consumers){
				if (consumer.try_put(data)){
					data = get();
				}
			}
		}
	}
	
};