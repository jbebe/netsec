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

#define BUFFER_SIZE 4

namespace Buffer {
	enum buffenum_t {
		IPV4_UDP = 0,
		IPV4_TCP = 1,
		IPV6_UDP = 2,
		IPV6_TCP = 3
	};
	struct buffer_t { /* 20byte */
		union {
			union {
				struct {
					unsigned char a, b, c, d;
				} parts;
				unsigned int full;
			} ipv4;
			union {
				struct {
					unsigned short a, b, c, d, e, f, g, h;
				} parts;
				unsigned int full[4];
			} ipv6;
		} ip;
		struct {
			unsigned short 	port; /* source port */
			unsigned short
							ttl : 8,
							type : 7,
							consumed : 1;
		} infos;
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
		data[i].infos.consumed = 1;
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
		while (data[getPos].infos.consumed == 1){
			getPos = (getPos + 1)%BUFFER_SIZE;
		}
		output = data[getPos];
		data[getPos].infos.consumed = 1;
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
