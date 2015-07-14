#pragma once

#include <cstdlib>
#include <pcap.h>

#include "PacketElem.hpp"

template <class Tconsumer>
class PacketProducer {

	PacketElem temp_packet;
	pcap_t* pcap_handle;
	
public:
	PacketProducer(const char *interface){
		char errbuf[PCAP_ERRBUF_SIZE] = "";
		pcap_handle = pcap_open_live(interface, PacketElem::MTU, 1, 1000, errbuf);
		if (strlen(errbuf) != 0) {
			throw std::system_error{std::error_code{}, errbuf};
		}
	}
	
	PacketElem *get(){
		struct pcap_pkthdr hdr;
		const uint8_t *data;
		while ((data = pcap_next(pcap_handle, &hdr)) == NULL);
		new (static_cast<void *>(&temp_packet)) PacketElem{data, hdr.caplen};
		return &temp_packet;
	}
	
	void run(std::vector<Tconsumer> *consumers){
		PacketElem *data = get();
		while (1){
			for (auto &consumer : *consumers){
				if (consumer.try_put(data)){
					data = get();
				}
			}
		}
	}
	
	~PacketProducer(){
		struct pcap_stat stat;
		pcap_stats(pcap_handle, &stat);
		dbg_printf("Received: %u, dropped: %u, dropped by nic: %u\n", 
				stat.ps_recv, stat.ps_drop, stat.ps_ifdrop);
		pcap_close(pcap_handle);
	}
};