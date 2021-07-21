#include "http_request.h"
#include "http_client.h"

http_request::http_request(http_client *client, const std::string &url, http_request::method method)
        : _http_client(client), _url(url), _method(method) { }

http_request &http_request::set_query(const params &query) {
    _query = query;
    return *this;
}

http_request &http_request::set_header(const std::pair<std::string, std::string> &header) {
    _headers.emplace(header.first, header.second);
    return *this;
}

http_request &http_request::set_header(const std::string &name, const std::string &value) {
    _headers.emplace(name, value);
    return *this;
}

http_request &http_request::set_body(const std::string &body) {
    return set_body("text/plain",
                    std::make_unique<const std::vector<uint8_t>>(body.begin(), body.end()));
}

http_request &http_request::set_body(const nlohmann::json &body) {
    std::string str = body.dump();
    return set_body("application/json",
                    std::make_unique<const std::vector<uint8_t>>(str.begin(), str.end()));
}

http_request &http_request::set_body(const params &body) {
    std::string str = _http_client->get_query_string(body);
    return set_body("application/x-www-form-urlencoded",
                    std::make_unique<const std::vector<uint8_t>>(str.begin(), str.end()));
}

http_request &http_request::set_body(byte_array body) {
    return set_body("application/octet-stream", std::move(body));
}

std::unique_ptr<const http_response> http_request::send() {
    return _http_client->send(*this);
}

http_request &http_request::set_body(const std::string &content_type, byte_array body) {
    if (_body != nullptr)
        throw std::runtime_error("This request already has a body!");
    if (_method == get)
        throw std::runtime_error("GET request cannot have a body!");

    set_header("Content-Type", content_type);
    _body = std::move(body);

    return *this;
}
