#ifndef PARSER_HPP_
#define PARSER_HPP_

#include <stdio.h>
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
	struct user_t {
		enum TTL_OS ttl;
	};
	using google::sparse_hash_map;
	using std::queue;
	sparse_hash_map<int, queue<user_t> > userDB;
	void *start(void *);
	unsigned int ipv6toSCH(unsigned short c, unsigned short d){
#if __BYTE_ORDER == __BIG_ENDIAN
	unsigned int out = 0x98420000;
	out |= (((c&0x0f00) >> 8)*100 + ((c&0x00f0) >> 4)*10 + (c&0x000f)) << 8;
	out |= ((d&0x0f00) >> 8)*100 + ((d&0x00f0) >> 4)*10 + (d&0x000f);
#else
	unsigned int out = 0x00004298;
	out |= (((c&0x0f00) >> 8)*100 + ((c&0x00f0) >> 4)*10 + (c&0x000f)) << 16;
	out |= (((d&0x0f00) >> 8)*100 + ((d&0x00f0) >> 4)*10 + (d&0x000f)) << 24;
#endif
	return out;
	}
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
				union {
					struct {
						unsigned char a, b, c, d;
					};
					unsigned int ip;
				} out;
				out.ip = ipv6toSCH(tmp.ip.ipv6.parts.f, tmp.ip.ipv6.parts.g);
				printf("%04x:%04x:%04x:%04x:%04x:%04x:%04x:%04x"
						" -> %3d.%3d.%3d.%3d "
						"port:%5d ttl:%3d load:%5.1f%%\n",
					tmp.ip.ipv6.parts.a, tmp.ip.ipv6.parts.b, tmp.ip.ipv6.parts.c, tmp.ip.ipv6.parts.d,
					tmp.ip.ipv6.parts.e, tmp.ip.ipv6.parts.f, tmp.ip.ipv6.parts.g, tmp.ip.ipv6.parts.h,
					out.a, out.b, out.c, out.d,
					ntohs(tmp.infos.port), tmp.infos.ttl, ((float)consumableCnt/BUFFER_SIZE)*100.0f);
				break;
		}
	}
	puts("parser loop: end");
	return 0;
}
#endif /* PARSER_HPP_ */
