#ifndef SAVE_SWITCH_HTTP_RESPONSE_H
#define SAVE_SWITCH_HTTP_RESPONSE_H

#include <vector>
#include <string>
#include <unordered_map>
#include <nlohmann/json.hpp>
#include "http_types.h"

class http_response {
    friend class http_client;

    int _code;
    byte_array _data = std::make_unique<const std::vector<uint8_t>>();
    headers _headers;

public:
    [[nodiscard]] int code() const;
    bool ok() const;

    [[nodiscard]] bool has_header(const std::string &name) const;
    [[nodiscard]] std::string get_header(const std::string &name) const;

    [[nodiscard]] std::string string() const;
    [[nodiscard]] nlohmann::json json() const;
};

#endif
