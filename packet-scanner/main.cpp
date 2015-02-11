//#define DEBUG

#include "pthread.h"
#include "scanner.hpp"
#include "buffer.hpp"
#include "parser.hpp"
#include "signals.hpp"

/*
 * INFOS
 * - Whoopsie daisy. You have to run the whole program as root
 *   in order to see all devs (eth0/wlan0)
 */

int main() {
#ifdef DEBUG
	Buffer::init();
	OS::init();
	pthread_create(&scannerThread, NULL, DummyScanner::start, NULL);
	pthread_create(&parserThread, NULL, Parser::start, NULL);

	pthread_join(scannerThread, NULL);
	pthread_join(parserThread, NULL);
#else
	//Scanner::init("eth0");
	Scanner::init("wlan0");
	Buffer::init();
	OS::init();

	pthread_t scannerThread, parserThread;
	pthread_create(&scannerThread, NULL, Scanner::start, NULL);
	pthread_create(&parserThread, NULL, Parser::start, NULL);

	pthread_join(scannerThread, NULL);
	pthread_join(parserThread, NULL);
#endif
	return 0;
}
