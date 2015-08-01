#pragma once

#include <mutex>
#include <condition_variable>

/*
 basic semaphore implementation
 semaphore helps to control waiting threads
 avoiding the problem of conditional_variable 
 when they might want to wait AFTER a notify
 */
class Semaphore {
	
	std::mutex m;
	std::condition_variable cv;
	bool ready = false;
	
public:
	/*
	 notifies every waiting thread
	 */
	void notify(){
		std::lock_guard<std::mutex> lg(m);
		ready = true;
		cv.notify_all();
	}
	
	/*
	 resets the flag back to 'red sign'
	 */
	void reset(){
		std::lock_guard<std::mutex> lg(m);
		ready = false;
	}
	
	/*
	 thread waits here until flag is true
	 */
	void wait(){
		std::unique_lock<std::mutex> ul(m);
		while (!ready){
			cv.wait(ul);
		}
	}
	
};
