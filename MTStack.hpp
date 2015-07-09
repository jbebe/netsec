#pragma once

#include <mutex>
#include <condition_variable>

#include "debug.hpp"

template <typename Telem = int, int Tcapacity = 8>
class MTStack {
private:
	Telem buffer[Tcapacity];
	std::mutex m;
	int pos;
	
	std::mutex cv_m;
	std::condition_variable cv;
	
public:
	MTStack(): pos{-1} {}
	
	Telem get(){
		std::unique_lock<std::mutex> ul{m};
		if /*empty*/ (pos == -1){
			ul.unlock();
			{
				//dbg_printf("get wait\n");
				std::unique_lock<std::mutex> cv_ul{cv_m};
				cv.wait(cv_ul, [this]{ return !this->empty(); });
				//dbg_printf("get resumed\n");
			}
			ul.lock();
		}
		cv.notify_one();
		return buffer[pos--];
	}
	
	void put(Telem data){
		std::unique_lock<std::mutex> ul{m};
		if /*full*/ (pos == Tcapacity - 1){
			ul.unlock();
			{
				//dbg_printf("put wait\n");
				std::unique_lock<std::mutex> cv_ul{cv_m};
				cv.wait(cv_ul, [this]{ return !this->full(); });
				//dbg_printf("put resumed\n");
			}
			ul.lock();
		}
		buffer[++pos] = data;
		cv.notify_one();
	}

	size_t size(){
		std::lock_guard<std::mutex> lg{m};
		return static_cast<size_t>(pos + 1);
	}
	
	bool empty(){ 
		return size() == 0; 
	}
	
	bool full(){
		return size() == Tcapacity; 
	}
};