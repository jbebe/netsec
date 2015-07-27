#pragma once

#include <cstdint>
#include <string>

class IPv46 {

private:
	union {
		uint64_t fmt_64[2];
		uint32_t fmt_32[4];
		uint16_t fmt_16[8];
		uint8_t  fmt_8[16];
		struct {
			uint64_t zero;    /* 0x0000000000000000 */
			uint32_t leading; /* 0x0000ffff */
			union {
			struct {
			uint8_t addr1; /* 1st byte */
			uint8_t addr2; /* 2nd byte */
			uint8_t addr3; /* 3rd byte */
			uint8_t addr4; /* 4th byte */
			};
			uint32_t addr;    /* ipv4 address */
			};
		} ipv4_mapped;
	};

	std::string toIPv4() const {
		char buff[16];
		snprintf(buff, 16, "%hhu.%hhu.%hhu.%hhu", 
			ipv4_mapped.addr1,
			ipv4_mapped.addr2,
			ipv4_mapped.addr3,
			ipv4_mapped.addr4
		);
		return std::string{buff};
	}
	
	std::string toIPv6() const {
		std::stringstream ss;
		for (int i = 0; i < 7; i++){
			ss << fmt_16[i] << ":";
		}
		ss << fmt_16[7];
		return ss.str();
	}
	
public:
	IPv46(const uint32_t *ptr = nullptr){
		if (ptr != nullptr){
			memcpy(fmt_32, ptr, sizeof(fmt_32));
		}
	}
	
	static IPv46 fromIPv4(uint32_t addr){
		const uint32_t tmp[4] = {0, 0, 0xffff, addr};
		return IPv46(tmp);
	}
	
	bool operator == (const IPv46 &rhs) const {
		return memcmp(fmt_32, rhs.fmt_32, sizeof(fmt_32)) == 0;
	}
	
	IPv46 &operator = (const IPv46 &rhs){
		memcpy(fmt_32, rhs.fmt_32, sizeof(fmt_32));
		return *this;
	}
	
	std::string str() const {
		if (ipv4_mapped.zero == 0 && ipv4_mapped.leading == 0xffff){
			return toIPv4();
		}
		else {
			return toIPv6();
		}
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