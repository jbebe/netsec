#pragma once

#include <cstdint>

#include "IPv46.hpp"

struct ParsedPacketElem {

	struct {
		IPv46 src_addr;
		IPv46 dest_addr;
		uint8_t ttl;
		uint16_t id;
	} ip_layer;

	struct {
		uint16_t src_sport;
		uint16_t dest_port;
	} tcp_layer;

	struct {
	} app_layer;
	
};
