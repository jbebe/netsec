#ifndef BUFFER_HPP_
#define BUFFER_HPP_

#include <pthread.h>
#include <string.h>
#include <unistd.h>
#include "scanner.hpp"

/*
 * INFOS
 * - Buffer is actually done.
 * - Cannot think of any improvements for now.
 */

#define BUFFER_SIZE 16

namespace Buffer {
	enum buffenum_t {
		IPV4_UDP,
		IPV4_TCP,
		IPV6_UDP,
		IPV6_TCP
	};
	struct buffer_t {
		unsigned int ip; /* source ip */
		unsigned short port; /* source port */
		int ttl;
		bool consumed;

		buffenum_t type; /* debug */
	};

	pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
	pthread_cond_t 	condition;
	buffer_t 		data[BUFFER_SIZE];
	int 			putPos = 0;
	int 			getPos = 0;
	int 			consumableCnt = 0;

	void init();
	void put(buffer_t input);
	buffer_t get();
	
	void lock();
	void unlock();
	void wait();
	void notify();
}

void Buffer::init(){
	for (int i = 0; i < BUFFER_SIZE; i++){
		data[i].consumed = true;
	}
	pthread_mutex_init(&mutex, NULL);
	pthread_cond_init(&condition, NULL);
}
void Buffer::put(buffer_t input){
	lock();
	{
		if (consumableCnt >= BUFFER_SIZE){
			wait();
		}
		data[putPos] = input;
		consumableCnt++;
		notify();
	}
	unlock();
	putPos = (putPos + 1)%BUFFER_SIZE;
}
Buffer::buffer_t Buffer::get(){
	buffer_t output;
	lock();
	{
		if (consumableCnt <= 0){
			wait();
		}
		while (data[getPos].consumed == true){
			getPos = (getPos + 1)%BUFFER_SIZE;
		}
		output = data[getPos];
		data[getPos].consumed = true;
		consumableCnt--;
		notify();
	}
	unlock();
	getPos = (getPos + 1)%BUFFER_SIZE;
	return output;
}
void Buffer::lock(){
	pthread_mutex_lock(&mutex);
}
void Buffer::unlock(){
	pthread_mutex_unlock(&mutex);
}
void Buffer::notify(){
	pthread_cond_signal(&condition);
}
void Buffer::wait(){
	pthread_cond_wait(&condition, &mutex);
}
#endif
