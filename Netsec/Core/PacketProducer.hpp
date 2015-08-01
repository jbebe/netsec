#pragma once

#include <cstdlib>
#include <pcap.h>

#include "../../Globals.hpp"
#include "../RawPacketElem.hpp"
#include "PacketConsumer.hpp"

class PacketProducer {
	pcap_t* pcap_handle;
	uint8_t hdr_len;
	
public:
	PacketProducer(): pcap_handle{nullptr} {}
	
	PacketProducer(const char *interface){
		char errbuf[PCAP_ERRBUF_SIZE] = "";
		pcap_handle = pcap_open_live(interface, RawPacketElem::MTU, 1, 1000, errbuf);
		if (strlen(errbuf) != 0) {
			throw std::system_error{std::error_code{}, errbuf};
		}
		hdr_len = getFrameLength(pcap_datalink(pcap_handle));
	}
	
	static PacketProducer debugInstance(const char *interface, const char *pcap_filter){
		PacketProducer pp{interface};
		struct bpf_program fp;
		pcap_activate(pp.pcap_handle);
		pcap_compile(pp.pcap_handle, &fp, pcap_filter, 1, PCAP_NETMASK_UNKNOWN);
		pcap_setfilter(pp.pcap_handle, &fp);
		return pp;
	}
	
	PacketProducer(PacketProducer &&rhs)
	: pcap_handle{rhs.pcap_handle}, hdr_len{rhs.hdr_len} 
	{
		rhs.pcap_handle = nullptr;
	}
	
	PacketProducer(const PacketProducer &)   = delete;
	void operator = (const PacketProducer &) = delete;
	
	void operator = (PacketProducer &&rhs){
		if (pcap_handle != nullptr){
			dbg_printf("pcap_handle was already set!\n");
		}
		pcap_handle = rhs.pcap_handle;
		hdr_len = rhs.hdr_len;
		rhs.pcap_handle = nullptr;
	}
	
	void get(RawPacketElem *temp_packet){
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
		*temp_packet = RawPacketElem{data, hdr.caplen};
	}
	
	void run(std::vector<PacketConsumer> *consumers){
		RawPacketElem data;
		get(&data);
		while (RUN_PRODUCER){
			for (auto &consumer : *consumers){
				if (consumer.try_put(&data)){
					get(&data);
				}
			}
		}
		RUN_CONSUMER.store(false);
		for (auto &consumer : *consumers){
			// wake consumers
			consumer.put(&data);
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
	
	~PacketProducer(){
		if (pcap_handle != nullptr){
			struct pcap_stat stat;
			pcap_stats(pcap_handle, &stat);
			dbg_printf("Received: %u, dropped: %u, dropped by nic: %u\n", 
					stat.ps_recv, stat.ps_drop, stat.ps_ifdrop);
			pcap_close(pcap_handle);
		}
	}
};