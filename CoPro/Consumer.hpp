#pragma once

#include <string>

#include "../debug.hpp"
#include "MTStack.hpp"

/*
 Consumer class is the consumer side 
 of a single-consumer - single producer data exchange structure 
 where consumer holds the data as a member variable.
 
 The current role of Consumer class is to be an interface-dummy 
 to test the stability of MTStack.
 */

template <typename Telem, int Tcapacity>
class Consumer {

	MTStack<Telem, Tcapacity> mtstack;
	
public:
	/*
	 std::vector uses move ctor so it is enabled in a limited way
	 */
	Consumer(Consumer &&moved_consumer): Consumer() {}
	
	/*
	 these should not happen
	 */
	Consumer(const Consumer &)         = delete;
	void operator = (const Consumer &) = delete;
	void operator = (Consumer &&)      = delete;

	/*
	 put places data on the mtstack and stays in the mutex lock
	 until get unlocks the mutex
	 */
	void put(Telem data){
		mtstack.put(data);
	}
	
	/*
	 try_put returns false if placement was unsuccessful and true otherwise
	 it does not wait in mutex lock thus make the program faster
	 */
	bool try_put(Telem *data){
		return mtstack.try_put(data);
	}
	
	/*
	 get turns back with data_in 
	 which is loaded with an element from mtstack
	 it waits in a mutex lock until put unlocks the mutex
	 */
	void get(Telem *data_in){
		mtstack.get(data_in);
	}
	
	/*
	 run is used to be called as a thread 
	 so the cpu will handle more consumers at once
	 */
	void run(){
		while (1){
			Telem data;
			get(&data);
			
			/*
			 basic log function that relies on that Telem 
			 has an 'operator std::string' function
			 */
			dbg_printf("%s\n", std::string{data}.c_str());
		}
	}
	
};
