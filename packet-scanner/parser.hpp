#ifndef PARSER_HPP_
#define PARSER_HPP_

#include <map>
#include "postgre.hpp"
#include "signals.hpp"

/*
 * INFOS
 * - Parser at this stage does not do any work
 * - This namespace should contain all the methods for NAT detection
 * - We should expand it with fruther classes/functions
 */

#define HISTORY_SIZE 64
#define DB_SIZE 2000

namespace Parser {
	/*
	 * Parser DB is a table of known ip -> ttl pairs (and hopefully other ip/tcp infos)
	 * We need to find an optimal jumptable for SCH dorm ip-s. (simple array, maybe we have to reinvent the wheel)
	 * (e.g. xxx.xxx.xx[x.xxx] <--- storing only these bits of the ip)
	 * and since we know that only 2000 users are present it can be fixed and we don't need linked list
	 */

	Postgre pgre;

	void *start(void *);
}

void *Parser::start(void *){
	using namespace Buffer;
	for (;;){
		buffer_t tmp = Buffer::get();

		switch (tmp.type) {
			case IPV4_TCP:
				printf("ipv4/TCP ");
				break;
			case IPV4_UDP:
				printf("ipv4/UDP ");
				break;
			case IPV6_TCP:
				printf("ipv6/TCP ");
				break;
			case IPV6_UDP:
				printf("ipv6/UDP ");
				break;
		}
		tmp.ip = ntohl(tmp.ip);
		printf("%3d.%3d.%3d.%3d port:%5d ttl:%d\n",
			(tmp.ip&0xff000000)>>24, (tmp.ip&0xff0000)>>16, (tmp.ip&0xff00)>>8, (tmp.ip&0xff),
			ntohs(tmp.port), tmp.ttl);
	}
	puts("parser loop: end");
	return 0;
}
#endif /* PARSER_HPP_ */
