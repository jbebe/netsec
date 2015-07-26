#pragma once

#include <cstdint>

#include "IPv46.hpp"

struct ParsedPacketElem {
	
	struct {
		IPv46 src_addr;
		IPv46 dest_addr;
		uint8_t ttl;
	} ip_layer;

	struct {
		
	} tcp_layer;

	struct {

	} app_layer;

	bool valid = false;

};
