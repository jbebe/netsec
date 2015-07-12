#pragma once

#include <mutex>
#include <condition_variable>
#include <type_traits>

#include "../debug.hpp"
#include "Semaphore.hpp"

template <typename Telem, int Tcapacity>
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