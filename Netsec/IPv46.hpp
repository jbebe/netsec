#pragma once

#include <cstdint>

class IPv46 {

private:
	union {
		uint32_t fmt_32[4];
		uint8_t fmt_8[16];
	};
	
public:
	IPv46(const uint32_t *ptr = nullptr){
		if (ptr != nullptr){
			memcpy(fmt_32, ptr, sizeof(fmt_32));
		}
	}
	
	static IPv46 fromIPv4(const in_addr *addr){
		const uint32_t tmp[4] = {0, 0, 0xffff, addr->s_addr};
		return IPv46(tmp);
	}
	
	bool operator == (const IPv46 &rhs) const {
		return memcmp(fmt_32, rhs.fmt_32, sizeof(fmt_32)) == 0;
	}
	
	IPv46 &operator = (const IPv46 &rhs){
		memcpy(fmt_32, rhs.fmt_32, sizeof(fmt_32));
		return *this;
	}
	
	class Hash {
		using LL = long long;
	public: 
		std::size_t operator()(const IPv46& addr) const {
			auto hash64 = std::hash<LL>{};
			const uint32_t *addr_ptr32 = addr.fmt_32;
			return hash64((LL)addr_ptr32[0] | ((LL)addr_ptr32[1] << 32)) 
					^ hash64((LL)addr_ptr32[2] | ((LL)addr_ptr32[3] << 32));
		}
	};
	
};