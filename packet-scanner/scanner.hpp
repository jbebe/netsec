#ifndef SCANNER_HPP_
#define SCANNER_HPP_

#include <pcap.h>
#include <netinet/ip.h>
#include <netinet/ip6.h>
#include <netinet/tcp.h>
#include <netinet/udp.h>
#include "buffer.hpp"
#include <stdlib.h>

/*
 * INFOS
 * - If we need more parser thread, callback_handler should throw
 *   to different 1v1 buffers for the sake of no double locking
 */

namespace Scanner {
	const int 		PACKET_LEN = 128;
	const int 		PROMISC_MODE = 1;

	pcap_t 			*pcapHandle;
	unsigned char 	frameHeaderLen = 0;
	char 			errbuff[0xff];

	void init(const char *interface);
	void *start(void *);
	void stop();
	void callback_handler(unsigned char *args, pcap_pkthdr *header, unsigned char *packet);
	void getFrameLength(unsigned int dataLinkType);
}

void Scanner::init(const char *interface){
	pcapHandle = pcap_open_live(interface, PACKET_LEN, PROMISC_MODE, 1, errbuff);
	if (pcapHandle == NULL){
		puts("Cannot create pcap handle");
		exit(0);
	}
	getFrameLength(pcap_datalink(pcapHandle));
}
void *Scanner::start(void *){
	pcap_loop(pcapHandle, -1, (pcap_handler)&callback_handler, 0);
	puts("pcap_loop: end");
	return 0;
}
void Scanner::stop(){
	pcap_breakloop(pcapHandle);
	pcap_close(pcapHandle);
}
void Scanner::callback_handler(unsigned char *args, pcap_pkthdr *header, unsigned char *packet){
	using namespace Buffer;
	buffer_t output;
	output.consumed = false;

	iphdr *ipv4 = (iphdr *)(packet + frameHeaderLen);

	if (ipv4->version == /*IPv*/4){
		output.ttl = ipv4->ttl;
		output.ip = ipv4->saddr;

		if (ipv4->protocol == IPPROTO_TCP){
			tcphdr *tcp = (tcphdr *)(ipv4 + (ipv4->ihl * 4));
			output.port = tcp->source;

			output.type = IPV4_TCP;
		}
		else if (ipv4->protocol == IPPROTO_UDP){
			udphdr *udp = (udphdr *)(ipv4 + (ipv4->ihl * 4));
			output.port = udp->source;

			output.type = IPV4_UDP;
		}
		put(output);
	}
	else if (ipv4->version == /*IPv*/6){
		ip6_hdr *ipv6 = (ip6_hdr *)ipv4;
		output.ttl = ipv6->ip6_ctlun.ip6_un1.ip6_un1_hlim;
		output.ip = 0xffffffff;

		if (ipv6->ip6_ctlun.ip6_un1.ip6_un1_nxt == IPPROTO_TCP){
			tcphdr *tcp = (tcphdr *)(ipv4 + (ipv4->ihl * 4));
			output.port = tcp->source;

			output.type = IPV6_TCP;
		}
		else if (ipv6->ip6_ctlun.ip6_un1.ip6_un1_nxt == IPPROTO_UDP){
			udphdr *udp = (udphdr *)(ipv4 + (ipv4->ihl * 4));
			output.port = udp->source;

			output.type = IPV6_UDP;
		}
		put(output);
	}
}
void Scanner::getFrameLength(unsigned int dataLinkType){
	switch(dataLinkType){
		case DLT_RAW: 							  break;
		case DLT_SLIP:       frameHeaderLen = 16; break;
		case DLT_EN10MB:     frameHeaderLen = 14; break;

		case DLT_LOOP:
		case DLT_NULL:
		case DLT_PPP:        frameHeaderLen =  4; break;
		case DLT_IEEE802:    frameHeaderLen = 22; break;
		case DLT_PPP_ETHER:  frameHeaderLen =  8; break;
		case DLT_PFLOG:      frameHeaderLen = 28; break;
		case DLT_LINUX_SLL:  frameHeaderLen = 16; break;
		case DLT_SLIP_BSDOS: frameHeaderLen = 24; break;
		case DLT_PPP_BSDOS:  frameHeaderLen = 24; break;

		// unknown datalink type
		case DLT_PPP_SERIAL:
		default: 								  break;
	}
}

namespace DummyScanner {
	using namespace Buffer;
	void *start(void *){
		for (;;){
			buffer_t output;
			int r = (rand()%5+1)*100000;
			output.consumed = false;
			output.ttl = 64;
			put(output);
			usleep(r);
		}
		return NULL;
	}
}

#endif
