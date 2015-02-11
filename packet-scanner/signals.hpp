#ifndef SIGNALS_HPP_
#define SIGNALS_HPP_

#include <signal.h>
#include "scanner.hpp"

/*
 * INFOS
 * - linux specific signaling / exit handling
 * - Cannot think of any improvements for now.
 */

namespace OS {
	void init();
	void int_sig(int);
	void term_sig(int);
	void segv_sig(int);
	void shutdown(){
		Scanner::stop();
		sleep(1);
		exit(0);
	}
}

void OS::init(){
	signal(SIGSEGV, &segv_sig);
	signal(SIGINT, &int_sig);
	signal(SIGTERM, &term_sig);
}
void OS::int_sig(int) {
	printf("caught SIGINT signal, exiting...\n");
	shutdown();
}
void OS::term_sig(int) {
	printf("caught SIGTERM signal, exiting...\n");
	shutdown();
}
void OS::segv_sig(int) {
	printf("caught SIGSEGV signal, exiting...\n");
	shutdown();
}
#endif
