#pragma once

#include <cstdint>
#include <sstream>

#include "../debug.hpp"
#include "EvaluatorInfo.hpp"
#include "ParsedPacketElem.hpp"

class NatDetect {

public:
	void operator () (std::vector<ParsedPacketElem>& packets, EvaluatorInfo& info){
		info.timestamp = std::time(0);
		info.type = "NAT";
		for (auto &packet : packets){
			uint8_t ttl = packet.ip_layer.ttl;
			if ((ttl & (ttl-1)) != 0){
				std::stringstream ss;
				ss << "TTL was: " << static_cast<uint32_t>(packet.ip_layer.ttl) << " which is suspicious";
				info.info = ss.str();
				info.probability = .8f;
				return;
			}
		}
	}

};
