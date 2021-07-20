#ifndef SAVE_SWITCH_UTILS_H
#define SAVE_SWITCH_UTILS_H

#include <string>

class utils {
public:
    static std::string to_hex_string(uint16_t i);
    static std::string to_hex_string(int i);
    static std::string to_hex_string(uint64_t i);

};

#endif
