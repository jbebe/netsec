#pragma once

#include <cstring>
#include <csignal>

#include "Globals.hpp"
#include "debug.hpp"

void score(int s){
	dbg_printf("Score: %d\n", performance_counter.load());
}

void init_signals(){
	
	struct sigaction sa;
	memset(&sa, 0, sizeof(sa));
	sa.sa_handler = score;
	
	//sigaction(SIGTERM, &sa, NULL);
	//sigaction(SIGINT, &sa, NULL);
	sigaction(SIGKILL, &sa, NULL);
	
	//sigaction(SIGSEGV, &sa, NULL);
}
