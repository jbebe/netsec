#pragma once

#include <cstring>
#include <csignal>

#include "Globals.hpp"

void sig_fn(int s){
	// TODO
}

void init_signals(){
	
	struct sigaction sa;
	memset(&sa, 0, sizeof(sa));
	sa.sa_handler = sig_fn;
	
	//sigaction(SIGTERM, &sa, NULL);
	//sigaction(SIGINT, &sa, NULL);
	//sigaction(SIGKILL, &sa, NULL);
	
	//sigaction(SIGSEGV, &sa, NULL);
}
