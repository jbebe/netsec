#pragma once

#include <cstdint>

struct ParsedPacketElem {
	
	struct {
		uint8_t ttl;
	} ip;

	struct {

	} tcp;

	struct {

	} app;
	
};