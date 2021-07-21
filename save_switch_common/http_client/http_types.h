#ifndef SAVE_SWITCH_HTTP_TYPES_H
#define SAVE_SWITCH_HTTP_TYPES_H

#include <string>
#include <unordered_map>
#include <nlohmann/json.hpp>

typedef nlohmann::json json;
typedef std::unordered_map<std::string, std::string> headers;
typedef std::unordered_map<std::string, std::string> params;
typedef std::unique_ptr<const std::vector<uint8_t>> byte_array;

#endif
