#pragma once

#include <mutex>
#include <condition_variable>

class Semaphore {
	
	std::mutex m;
	std::condition_variable cv;
	bool ready = false;
	
public:
	void notify(){
		std::lock_guard<std::mutex> lg(m);
		ready = true;
		cv.notify_all();
	}
	void reset(){
		std::lock_guard<std::mutex> lg(m);
		ready = false;
	}
	void wait() {
		std::unique_lock<std::mutex> ul(m);
		while (!ready){
			cv.wait(ul);
		}
	}
	
};