#pragma once

#include <cstdint>

class PacketElem {

public:
	static constexpr unsigned short MTU = 1500;
	
private:
	uint8_t data[MTU];
	size_t size;
	
public:
	PacketElem(const uint8_t *data_in, size_t size): size{size} {
		memcpy(data, data_in, size);
	}
	
	size_t getSize(){
		return size;
	}
	
	uint8_t *getData(){
		return data;
	}
	
};