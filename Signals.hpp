#pragma once

#include <cstring>
#include <csignal>

#include "Config.hpp"
#include "_debug.hpp"

namespace NETSEC {

void basic_sighandler(int s/*ignal*/){
	dbg_printf("signal %d caught\n", s);
	STOP = true;
}

void init_signals(){
	
	struct sigaction sa;
	memset(&sa, 0, sizeof(sa));
	sa.sa_handler = basic_sighandler;
	
	sigaction(SIGTERM, &sa, NULL);
	sigaction(SIGINT, &sa, NULL);
	/*
	sigaction(SIGSEGV, &sa, NULL);
	sigaction(SIGKILL, &sa, NULL);
	*/
}

}