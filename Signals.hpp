#pragma once

#include <cstring>
#include <csignal>

#include "Globals.hpp"
#include "debug.hpp"

void exit_netsec(int s){
	RUN_PRODUCER.store(false);
}

void init_signals(){
	
	struct sigaction sa;
	memset(&sa, 0, sizeof(sa));
	sa.sa_handler = exit_netsec;
	
	//sigaction(SIGTERM, &sa, NULL);
	sigaction(SIGINT, &sa, NULL);
	//sigaction(SIGKILL, &sa, NULL);
	
	//sigaction(SIGSEGV, &sa, NULL);
}
