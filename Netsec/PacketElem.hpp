#pragma once

#include <string>
#include <sstream>
#include <cstdint>

class PacketElem {

public:
	static constexpr unsigned short MTU = 1500;
	
private:
	uint8_t data[MTU];
	size_t size;
	
public:
	PacketElem() {}
	
	PacketElem(const uint8_t *data_in, size_t size): size{size} {
		if (size <= MTU){
			memcpy(data, data_in, size);
		}
		else {
			dbg_printf("%s(%d): size is bigger than MTU!\n", __FILE__,  __LINE__);
		}
	}
	
	size_t getSize(){
		return size;
	}
	
	uint8_t *getData(){
		return data;
	}
	
	operator std::string (){
		char buff[256];
		char *buff_ptr = buff + sprintf(buff, "data: ");
		for (int i = 0; i < std::min<int>(size, 5); i++){
			buff_ptr += sprintf(buff_ptr, "%02X", data[i]);
		}
		sprintf(buff_ptr, "... size: %zu\n", size);
		return std::string{buff};
	}
	
};