#pragma once

#include <mutex>

#include "Semaphore.hpp"

/*
 MTStack stands for Multi-Threaded Stack
 It supports a basic single producer single consumer system 
 with an additional try_put method supporting performance
 because the producer can cycle through multiple MTStack
 */

template <typename Telem, int Tcapacity>
class MTStack {
	
private:
	Telem buffer[Tcapacity];
	int pos;
	std::mutex m;

	Semaphore get_s;
	Semaphore put_s;
	
	static constexpr int POS_EMPTY = -1;
	static constexpr int POS_FULL = Tcapacity - 1;
	
public:
	MTStack(): pos{-1} {}
	
	/*
	 these should not happen
	 */
	MTStack(MTStack &&)               = delete;
	MTStack(const MTStack &)          = delete;
	void operator = (const MTStack &) = delete;
	void operator = (MTStack &&)      = delete;
	
	void get(Telem *data_in){
		std::unique_lock<std::mutex> ul{m};

		while (pos == POS_EMPTY){
			ul.unlock();
			get_s.wait();
			get_s.reset();
			ul.lock();
		}
		*data_in = buffer[pos--];
		if (pos == POS_FULL - 1){
			put_s.notify();
		}
	}
	
	/*
	 two put function will be instantiated 
	 put<true>: it is already locked, adopt the lock
	 put<> defaults to false: it has not been locked yet
	 */
	template <bool Tlocked = false>
	void put(Telem *data){
		typedef std::unique_lock<std::mutex> uniq_lck;
		uniq_lck ul = Tlocked ? uniq_lck{m, std::adopt_lock} : uniq_lck{m};

		while (pos == POS_FULL){
			ul.unlock();
			put_s.wait();
			put_s.reset();
			ul.lock();
		}
		buffer[++pos] = *data;
		if (pos == POS_EMPTY + 1){
			get_s.notify();
		}
	}
	
	bool try_put(Telem *data){
		if (m.try_lock() == false){
			return false;
		}
		if (pos == POS_FULL){
			m.unlock();
			return false;
		}
		put<true>(data);
		return true;
	}
	
};