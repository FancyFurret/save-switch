#include "http_response.h"
#include <save_switch_common/log.h>

int http_response::code() const {
    return _code;
}

bool http_response::has_header(const std::string &name) const {
    return _headers.contains(name);
}

std::string http_response::get_header(const std::string &name) const {
    return _headers.at(name);
}

std::string http_response::string() const {
    return std::string(_data.begin(), _data.end());
}

nlohmann::json http_response::json() const {
    return nlohmann::json::parse(string());
}

bool http_response::ok() const {
    return _code >= 200 && _code < 300;
}

