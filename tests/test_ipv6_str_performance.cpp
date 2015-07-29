#include <iostream>
#include <string>
#include <sstream>
#include <cstdint>
#include <chrono>
#include <boost/regex.hpp>

std::string results[] {
    "1:1:1:1:1:1:1:1",
    "0:1:0:1:0:1:0:1",
    "1:0:1:0:1:0:1:0",
    "1:1::1:1:1:1",
    "1::1:0:0:1",
    "1:0:0:1::1",
    "::1:1:1:1:1:1",
    "1:1:1:1:1:1::",
    "::1",
    "1::",
    "0:0:0:1::",
    "::"
};

uint16_t fmt_16_arr[][8] = {
    {1, 1, 1, 1, 1, 1, 1, 1}, // no zero
    {0, 1, 0, 1, 0, 1, 0, 1}, // 1 zeros
    {1, 0, 1, 0, 1, 0, 1, 0}, // 1 zeros #2
    {1, 1, 0, 0, 1, 1, 1, 1}, // 1 compress
    {1, 0, 0, 0, 1, 0, 0, 1}, // 2 compress 1st
    {1, 0, 0, 1, 0, 0, 0, 1}, // 2 compress 2nd
    {0, 0, 1, 1, 1, 1, 1, 1}, // 1 leading compress 
    {1, 1, 1, 1, 1, 1, 0, 0}, // 1 trailing compress 
    {0, 0, 0, 0, 0, 0, 0, 1}, // 1 leading compress #2
    {1, 0, 0, 0, 0, 0, 0, 0}, // 1 trailing compress #2
    {0, 0, 0, 1, 0, 0, 0, 0}, // 1 center compress
    {0, 0, 0, 0, 0, 0, 0, 0}  // all zero
};

inline void int_to_hex(char **str, uint16_t num){
    uint8_t nibble_1 = (num >> 12);
    uint8_t nibble_2 = ((num & 0x0f00) >> 8);
    uint8_t nibble_3 = ((num & 0x00f0) >> 4);
    uint8_t nibble_4 = (num & 0x000f);
    
    if (nibble_1 != 0){
        **str = nibble_1 + (nibble_1 < 10 ? '0' : 'a');
        (*str)++;
    }
    if (nibble_1|nibble_2 != 0){
        **str = nibble_2 + (nibble_2 < 10 ? '0' : 'a');
        (*str)++;
    }
    if (nibble_1|nibble_2|nibble_3 != 0){
        **str = nibble_3 + (nibble_3 < 10 ? '0' : 'a');
        (*str)++;
    }
    **str = nibble_4 + (nibble_4 < 10 ? '0' : 'a');
    (*str)++;
}

std::string toCompressedIPv6_no_sstream_no_sprintf(const uint16_t fmt_16[8]){
    uint8_t counter = 0;
    uint8_t max_value = 0;
    uint8_t max_place;
    char out[8*4 + 7*1 + 1];
    char *out_ptr = out;
    constexpr int ipv6_length = sizeof(fmt_16)/sizeof(char);

    // get position of 2 or more zeros
    for (int i = ipv6_length - 1; i >= 0; i--) {
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
            int_to_hex(&out_ptr, fmt_16[i]);
            *out_ptr++ = ':';
        }
        int_to_hex(&out_ptr, fmt_16[ipv6_length-1]);
    }        
    // print compressed form otherwise
    else {
        for (int i = 0; i < ipv6_length;) {
            if (i != max_place) {
                int_to_hex(&out_ptr, fmt_16[i]);
                if (i != ipv6_length - 1) *out_ptr++ = ':';
                i++;
            } else {
                if (max_place == 0){
                    *out_ptr++ = ':';
                    *out_ptr++ = ':';
                }
                else *out_ptr++ = ':';
                i += max_value;
            }
        }
    }
    *out_ptr = '\0';
    return out;
}

std::string toCompressedIPv6_no_sstream(const uint16_t fmt_16[8]){
    uint8_t counter = 0;
    uint8_t max_value = 0;
    uint8_t max_place;
    char out[8*4 + 7*1 + 1];
    char *out_ptr = out;
    constexpr int ipv6_length = sizeof(fmt_16)/sizeof(char);

    // get position of 2 or more zeros
    for (int i = ipv6_length - 1; i >= 0; i--) {
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
            out_ptr += sprintf(out_ptr, "%hx:", fmt_16[i]);
        }
        out_ptr += sprintf(out_ptr, "%hx", fmt_16[ipv6_length-1]);
    }        
    // print compressed form otherwise
    else {
        for (int i = 0; i < ipv6_length;) {
            if (i != max_place) {
                out_ptr += sprintf(out_ptr, "%hx", fmt_16[i]);
                if (i != ipv6_length - 1) *out_ptr++ = ':';
                i++;
            } else {
                if (max_place == 0){
                    *out_ptr++ = ':';
                    *out_ptr++ = ':';
                }
                else *out_ptr++ = ':';
                i += max_value;
            }
        }
    }
    *out_ptr = '\0';
    return out;
}

std::string toCompressedIPv6Regex(const uint16_t fmt_16[8]){
    const static boost::regex regex_obj("((?:(?:^|:)0+\\b){2,}):?(?!\\S*\\b\\1:0+\\b)(\\S*)");
    const static std::string regex_fmt{"::$2"};
    constexpr int ipv6_length = sizeof(fmt_16)/sizeof(char);
    std::stringstream out;
    for (int i = 0; i < ipv6_length - 1; i++) {
        out << std::hex << fmt_16[i];
        out.put(':');
    }
    out << std::hex << fmt_16[ipv6_length - 1];
    return boost::regex_replace(out.str(), regex_obj, regex_fmt);
}

std::string toCompressedIPv6(const uint16_t fmt_16[8]){
    uint8_t counter = 0;
    uint8_t max_value = 0;
    uint8_t max_place;
    std::stringstream out;
    constexpr int ipv6_length = sizeof(fmt_16)/sizeof(char);

    // get position of 2 or more zeros
    for (int i = ipv6_length - 1; i >= 0; i--) {
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
        for (int i = 0; i < ipv6_length - 1; i++) {
            out << std::hex << fmt_16[i];
            out.put(':');
        }
        out << std::hex << fmt_16[ipv6_length - 1];
    }        
    // print compressed form otherwise
    else {
        for (int i = 0; i < ipv6_length;) {
            if (i != max_place) {
                out << std::hex << fmt_16[i];
                if (i != ipv6_length - 1) out.put(':');
                i++;
            } else {
                if (max_place == 0) out << "::";
                else out.put(':');
                i += max_value;
            }
        }
    }
    return out.str();
}

void validate(std::string (*fn)(const uint16_t[8])){
    for (int i = 0; i < sizeof(fmt_16_arr)/sizeof(fmt_16_arr[0]); i++){
        std::string temp;
        temp = fn(fmt_16_arr[i]);
        if (temp.compare(results[i]) == 0){
            std::cout << "OK ";
        }
        else {
            std::cout << "\nERROR: " << results[i] << " (valid) " << temp << "\n";
        }
    }
    std::cout << std::endl;
}

void performance(std::string (*fn)(const uint16_t[8])){
    auto start = std::chrono::system_clock::now();
    
    for (int c = 0; c < 100000; c++){
        for (int i = 0; i < sizeof(fmt_16_arr)/sizeof(fmt_16_arr[0]); i++){
            std::string temp;
            temp = fn(fmt_16_arr[i]);
        }
    }
    
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now() - start);
    std::cout << "time: " << duration.count() << "ms" << std::endl;
}

int main(){
    std::cout << "\nmy implementation + no sstream + no sprintf:\n";
    validate(toCompressedIPv6_no_sstream_no_sprintf);
    performance(toCompressedIPv6_no_sstream_no_sprintf);
    std::cout << "\nmy implementation + no sstream:\n";
    validate(toCompressedIPv6_no_sstream);
    performance(toCompressedIPv6_no_sstream);
    std::cout << "\nmy implementation:\n";
    validate(toCompressedIPv6);
    performance(toCompressedIPv6);
    std::cout << "\nregex implementation:\n";
    validate(toCompressedIPv6Regex);
    performance(toCompressedIPv6Regex);
    
    return 0;
}
