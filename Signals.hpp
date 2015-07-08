#pragma once

#include <cstring>
#include <csignal>

#include "Globals.hpp"
#include "debug.hpp"

void basic_sighandler(int s/*ignal*/){
	dbg_printf("signal %d caught\n", s);
	RUN = false;
}

void init_signals(){
	
	struct sigaction sa;
	memset(&sa, 0, sizeof(sa));
	sa.sa_handler = basic_sighandler;
	
	sigaction(SIGTERM, &sa, NULL);
	sigaction(SIGINT, &sa, NULL);
	
	//sigaction(SIGSEGV, &sa, NULL);
	//sigaction(SIGKILL, &sa, NULL);
}
