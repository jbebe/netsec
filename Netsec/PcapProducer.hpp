#pragma once

#include <cstdlib>
#include <pcap.h>

#include "RawPacketElem.hpp"

template <class Tconsumer>
class PcapProducer {

	RawPacketElem temp_packet;
	pcap_t* pcap_handle;
	int hdr_len;
	
public:
	PcapProducer(const char *interface){
		char errbuf[PCAP_ERRBUF_SIZE] = "";
		pcap_handle = pcap_open_live(interface, RawPacketElem::MTU, 1, 1000, errbuf);
		if (strlen(errbuf) != 0) {
			throw std::system_error{std::error_code{}, errbuf};
		}
		hdr_len = getFrameLength(pcap_datalink(pcap_handle));
	}
	
	RawPacketElem *get(){
		struct pcap_pkthdr hdr;
		const uint8_t *data;
		while ((data = pcap_next(pcap_handle, &hdr)) == NULL);
		if (hdr.caplen < hdr_len){
			throw std::system_error{};
		}
		else {
			data += hdr_len;
			hdr.caplen -= hdr_len;
		}
		new (static_cast<void *>(&temp_packet)) RawPacketElem{data, hdr.caplen};
		return &temp_packet;
	}
	
	void run(std::vector<Tconsumer> *consumers){
		RawPacketElem *data = get();
		while (1){
			for (auto &consumer : *consumers){
				if (consumer.try_put(data)){
					data = get();
				}
			}
		}
	}
	
	static int getFrameLength(int dataLinkType){
		switch(dataLinkType){
			case DLT_PPP_SERIAL:
			case DLT_RAW:        return 0;
			case DLT_SLIP:       return 16;
			case DLT_EN10MB:     return 14;
			
			case DLT_LOOP:
			case DLT_NULL:
			case DLT_PPP:        return  4;
			case DLT_IEEE802:    return 22;
			case DLT_PPP_ETHER:  return  8;
			case DLT_PFLOG:      return 28;
			case DLT_LINUX_SLL:  return 16;
			case DLT_SLIP_BSDOS: return 24;
			case DLT_PPP_BSDOS:  return 24;
			default:             return 0;
		}
	}
	
	~PcapProducer(){
		struct pcap_stat stat;
		pcap_stats(pcap_handle, &stat);
		dbg_printf("Received: %u, dropped: %u, dropped by nic: %u\n", 
				stat.ps_recv, stat.ps_drop, stat.ps_ifdrop);
		pcap_close(pcap_handle);
	}
};