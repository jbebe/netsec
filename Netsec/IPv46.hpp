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
			uint32_t addr;    /* ipv4 address */
		} ipv4_mapped;
	};

	std::string toIPv4(uint32_t ipv4_addr) const {
		struct ipv4_bytes {
			uint8_t addr1; /* 1st byte */
			uint8_t addr2; /* 2nd byte */
			uint8_t addr3; /* 3rd byte */
			uint8_t addr4; /* 4th byte */
		};
		ipv4_bytes *bytes = reinterpret_cast<ipv4_bytes*>(&ipv4_addr);
		char buff[16];
		snprintf(buff, 16, "%hhu.%hhu.%hhu.%hhu", 
			bytes->addr1, bytes->addr2, 
			bytes->addr3, bytes->addr4
		);
		return std::string{buff};
	}
	
	static std::string toIPv6(const uint16_t fmt_16[8]){
		std::stringstream ss;
		for (int i = 0; i < 7; i++){
			ss << std::hex << fmt_16[i] << ":";
		}
		ss << fmt_16[7];
		return ss.str();
	}

	inline static void _short_to_hex(char **str, uint16_t num){
		uint8_t nibble_1 = (num >> 12);
		uint8_t nibble_2 = ((num & 0x0f00) >> 8);
		uint8_t nibble_3 = ((num & 0x00f0) >> 4);
		uint8_t nibble_4 = (num & 0x000f);

		if (nibble_1 != 0){
			**str = nibble_1 + (nibble_1 < 10 ? '0' : 'a'-10);
			(*str)++;
		}
		if ((nibble_1|nibble_2) != 0){
			**str = nibble_2 + (nibble_2 < 10 ? '0' : 'a'-10);
			(*str)++;
		}
		if ((nibble_1|nibble_2|nibble_3) != 0){
			**str = nibble_3 + (nibble_3 < 10 ? '0' : 'a'-10);
			(*str)++;
		}
		**str = nibble_4 + (nibble_4 < 10 ? '0' : 'a'-10);
		(*str)++;
	}

	inline static std::string toCompressedIPv6(const uint16_t fmt_16[8]){
		uint8_t counter = 0;
		uint8_t max_value = 0;
		uint8_t max_place;
		char out[8*4 + 7*1 + 1];
		char *out_ptr = out;
		constexpr int ipv6_length = sizeof(fmt_16)/sizeof(char);

		// get position of 2 or more zeros
		for (int i = ipv6_length - 1; i >= 0; i--){
			if (fmt_16[i] == 0) {
				counter++;
				if (counter > 1 && counter > max_value) {
					max_value = counter;
					max_place = i;
				}
			}
			else counter = 0;
		}
		// print full form if there is no pack of 2 or more zeros
		if (max_value == 0) {
			for (int i = 0; i < ipv6_length - 1; i++){
				_short_to_hex(&out_ptr, fmt_16[i]);
				*out_ptr++ = ':';
			}
			_short_to_hex(&out_ptr, fmt_16[ipv6_length-1]);
		}        
		// print compressed form otherwise
		else {
			for (int i = 0; i < ipv6_length;){
				if (i != max_place) {
					_short_to_hex(&out_ptr, fmt_16[i]);
					if (i != ipv6_length - 1) *out_ptr++ = ':';
					i++;
				} else {
					if (max_place == 0) *out_ptr++ = ':';
					*out_ptr++ = ':';
					i += max_value;
				}
			}
		}
		*out_ptr = '\0';
		return out;
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
		if (ipv4_mapped.zero == uint64_t{0} && ipv4_mapped.leading == 0xffff){
			return toIPv4(ipv4_mapped.addr);
		}
		else {
			return toCompressedIPv6(fmt_16);
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
