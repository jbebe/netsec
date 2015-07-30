#pragma once

#include <string>
#include <sstream>
#include <cstdint>

#include "../debug.hpp"

class RawPacketElem {

public:
	static constexpr unsigned short MTU = 1500;
	
private:
	uint8_t packet_data[MTU];
	size_t packet_size;
	
public:
	RawPacketElem() {}
	
	RawPacketElem(const uint8_t *data_in, size_t size): packet_size{size} {
		if (size <= MTU)
			memcpy(packet_data, data_in, size);
		else
			dbg_printf("%s(%d): size is bigger than MTU!\n");
	}
	
	size_t size() const {
		return packet_size;
	}
	
	uint8_t *data(){
		return packet_data;
	}
	
};