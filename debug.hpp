#pragma once

#pragma GCC diagnostic push 
#pragma GCC diagnostic ignored "-Wunused-variable"
#pragma GCC diagnostic ignored "-Wformat-security"

#include <cstdlib>
#include <cstdio>
#include <mutex>

std::mutex print_mutex;

template<typename... TArgs>
void dbg_printf(TArgs... args){
	static auto exec_once = [](){ setbuf(stdout, NULL); return 0; }();
	print_mutex.lock();
	printf(args...);
	//fflush(stdout);
	print_mutex.unlock();
}

#pragma GCC diagnostic pop
