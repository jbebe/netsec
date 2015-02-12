#ifndef PARSER_HPP_
#define PARSER_HPP_

#include <google/sparse_hash_map>
#include <queue>
#include "postgre.hpp"
#include "signals.hpp"

/*
 * INFOS
 * - Parser at this stage does not do any work
 * - This namespace should contain all the methods for NAT detection
 * - We should expand it with fruther classes/functions
 * Parser DB is a table of known ip -> ttl pairs (and hopefully other ip/tcp infos)
 * We need to find an optimal jumptable for SCH dorm ip-s. (simple array, maybe we have to reinvent the wheel)
 * (e.g. xxx.xxx.xx[x.xxx] <--- storing only these bits of the ip)
 * and since we know that only 2000 users are present it can be fixed and we don't need linked list
 * 254 + 1022 + 2046 = 3322 db user max
 */

#define HISTORY_SIZE 64
#define DB_SIZE 2000

namespace Parser {

	enum TTL_OS 	{ WIN95 = 32, WINNT = 128, BSD = 255, UNIX = 64 };
	/*enum WINDOW_OS 	{};*/

	struct user_t {
		enum TTL_OS ttl;
	};

	using google::sparse_hash_map;
	using std::queue;
	sparse_hash_map<int, queue<user_t> > userDB;

	void *start(void *);
}

void *Parser::start(void *){
	using namespace Buffer;
	for (;;){
		buffer_t tmp = Buffer::get();

		switch (tmp.infos.type) {
			case IPV4_TCP:
			case IPV4_UDP:
				printf("%3d.%3d.%3d.%3d port:%5d ttl:%3d load:%5.1f%%\n",
					tmp.ip.ipv4.parts.a,tmp.ip.ipv4.parts.b, tmp.ip.ipv4.parts.c, tmp.ip.ipv4.parts.d,
					ntohs(tmp.infos.port), tmp.infos.ttl, ((float)consumableCnt/BUFFER_SIZE)*100.0f);
				break;
			case IPV6_TCP:
			case IPV6_UDP:
				printf("%04x:%04x:%04x:%04x:%04x:%04x:%04x:%04x port:%5d ttl:%3d load:%5.1f%%\n",
					tmp.ip.ipv6.parts.a, tmp.ip.ipv6.parts.b, tmp.ip.ipv6.parts.c, tmp.ip.ipv6.parts.d,
					tmp.ip.ipv6.parts.e, tmp.ip.ipv6.parts.f, tmp.ip.ipv6.parts.g, tmp.ip.ipv6.parts.h,
					ntohs(tmp.infos.port), tmp.infos.ttl, ((float)consumableCnt/BUFFER_SIZE)*100.0f);
				break;
		}
	}
	puts("parser loop: end");
	return 0;
}
#endif /* PARSER_HPP_ */
