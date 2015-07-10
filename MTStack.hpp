#pragma once

#include <mutex>
#include <condition_variable>

#include "debug.hpp"
#include "Semaphore.hpp"

template <typename Telem = int, int Tcapacity = 8>
class MTStack {
private:
	Telem buffer[Tcapacity];
	std::mutex m;
	int pos;

	Semaphore get_s;
	Semaphore put_s;
	
	static constexpr int POS_EMPTY = -1;
	static constexpr int POS_FULL = Tcapacity - 1;
	
public:
	MTStack(): pos{-1} {}
	
	Telem get(){
		std::unique_lock<std::mutex> ul{m};
		while (pos == POS_EMPTY){
			ul.unlock();
			get_s.wait();
			get_s.reset();
			ul.lock();
		}
		Telem data = buffer[pos--];
		if (pos == POS_FULL - 1){
			put_s.notify();
		}
		return data;
	}
	
	void put(Telem data){
		std::unique_lock<std::mutex> ul{m};
		while (pos == POS_FULL){
			ul.unlock();
			put_s.wait();
			put_s.reset();
			ul.lock();
		}
		buffer[++pos] = data;
		if (pos == POS_EMPTY + 1){
			get_s.notify();
		}
	}
	
	bool try_put(Telem &data){
		std::unique_lock<std::mutex> ul{m, std::defer_lock};
		if (ul.try_lock() == false || pos == POS_FULL){
			return false;
		}
		ul.unlock();
		put(data);
		return true;
	}
	
};