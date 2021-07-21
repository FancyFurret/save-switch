#ifndef SAVE_SWITCH_HTTP_REQUEST_H
#define SAVE_SWITCH_HTTP_REQUEST_H

#include <string>
#include <unordered_map>
#include <nlohmann/json.hpp>
#include "http_response.h"
#include "http_types.h"

class http_client;

class http_request {
    friend class http_client;

public:
    enum method {
        get,
        post,
        put
    };

private:
    http_client *_http_client;

    std::string _url;
    method _method;

    headers _headers;

    bool _has_query = false;
    params _query;

    bool _has_body = false;
    std::unique_ptr<const std::vector<uint8_t>> _body = nullptr;

public:
    http_request(http_client *client, const std::string &url, method method);

    http_request &set_query(const params &query);

    http_request &set_header(const std::pair<std::string, std::string> &header);
    http_request &set_header(const std::string &name, const std::string &value);

    http_request &set_body(const std::string &body);
    http_request &set_body(const nlohmann::json &body);
    http_request &set_body(const params &body);
    http_request &set_body(std::unique_ptr<const std::vector<uint8_t>> body);

    std::unique_ptr<const http_response> send();

private:
    http_request &set_body(const std::string& content_type, std::unique_ptr<const std::vector<uint8_t>> body);
};

#endif
