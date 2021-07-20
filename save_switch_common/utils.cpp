#include "utils.h"
#include <sstream>

std::string utils::to_hex_string(uint16_t i) {
    std::stringstream ss;
    ss << std::hex << i;
    return ss.str();
}

std::string utils::to_hex_string(int i) {
    std::stringstream ss;
    ss << std::hex << i;
    return ss.str();
}

std::string utils::to_hex_string(uint64_t i) {
    std::stringstream ss;
    ss << std::hex << i;
    return ss.str();
}
