#pragma once

#include <mutex>

#include "Semaphore.hpp"

/*
 MTStack stands for Multi-Threaded Stack
 It is a lockful stack implementation
 */
template <typename Telem, int Tcapacity>
class MTStack {
	
private:
	Telem buffer[Tcapacity];
	/*
	 pos is the index of the stack
	 if the stack is empty pos waits at value -1
	 if the stack is full pos is at capacity-1
	 */
	int pos;
	static constexpr int POS_EMPTY = -1;
	static constexpr int POS_FULL = Tcapacity - 1;	
	
	/*
	 by definition these two are needed for lockful
	 consumer producer structure
	 */
	Semaphore get_sem;
	Semaphore put_sem;
	std::mutex m;
	
public:
	MTStack(): pos{-1} {}
	
	/*
	 these should not happen
	 not that any other operator should work 
	 but these are the most important
	 */
	MTStack(MTStack &&)               = delete;
	MTStack(const MTStack &)          = delete;
	void operator = (const MTStack &) = delete;
	void operator = (MTStack &&)      = delete;
	
	/*
	 get pops the data into the supplied pointer
	 */
	void get(Telem * const data_in){
		std::unique_lock<std::mutex> ul{m};

		while (pos == POS_EMPTY){
			ul.unlock();
			get_sem.wait();
			get_sem.reset();
			ul.lock();
		}
		*data_in = buffer[pos--];
		if (pos == POS_FULL - 1){
			put_sem.notify();
		}
	}
	
	/*
	 two put function will be instantiated 
	 put<true>: it is already locked, adopt the lock
	 put<false>: it has not been locked yet
	 */
	template <bool Tlocked = false>
	void put(const Telem * const data){
		typedef std::unique_lock<std::mutex> uniq_lck;
		uniq_lck ul = Tlocked ? uniq_lck{m, std::adopt_lock} : uniq_lck{m};

		while (pos == POS_FULL){
			ul.unlock();
			put_sem.wait();
			put_sem.reset();
			ul.lock();
		}
		buffer[++pos] = *data;
		if (pos == POS_EMPTY + 1){
			get_sem.notify();
		}
	}
	
	/*
	 try_put supports performance because the user 
	 can cycle through multiple MTStack objects if try_put returns false
	 
	 behaviour: try_put tries to lock the object mutex
	 if it fails it returns 
	 if it was not locked we check whether the stack is full
	 if it is full it returns
	 so try_put executes put if the object is unlocked and not full
	 */
	bool try_put(const Telem * const data){
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