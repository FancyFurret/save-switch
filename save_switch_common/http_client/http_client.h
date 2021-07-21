#ifndef SAVE_SWITCH_HTTP_CLIENT_H
#define SAVE_SWITCH_HTTP_CLIENT_H

#include <curl/curl.h>
#include <string>
#include <unordered_map>
#include <nlohmann/json.hpp>
#include "http_response.h"
#include "http_request.h"

class http_client {
    CURL *_curl = nullptr;
    CURLcode _res = CURLE_OK;
    headers _headers;

public:
    void init();
    virtual ~http_client();

    void set_auth_header(const std::string &value);

    http_request get(const std::string &url);
    http_request post(const std::string &url);
    http_request put(const std::string &url);
    http_request patch(const std::string &url);

    std::unique_ptr<const http_response> send(const http_request &request);

    [[nodiscard]] std::vector<std::string> get_header_list(const headers &headers) const;
    [[nodiscard]] std::string get_query_string(const params &params) const;
};

#endif
