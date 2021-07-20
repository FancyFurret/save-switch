#ifndef SAVE_SWITCH_HTTP_CLIENT_H
#define SAVE_SWITCH_HTTP_CLIENT_H

#include <curl/curl.h>
#include <string>
#include <unordered_map>
#include <nlohmann/json.hpp>

typedef nlohmann::json json;
typedef std::unordered_map<std::string, std::string> params;

class http_client {
    CURL *_curl;
    CURLcode _res;
    curl_slist *_user_headers;
    curl_slist *_headers;

public:
    void init();

    void set_header(const std::string &name, const std::string &value);

    std::string get_string(const std::string &url, const params &params);
    nlohmann::json get_json(const std::string &url, const params &params);

    std::string post_string(const std::string &url, const std::string &body);
    std::string post_string(const std::string &url, const params &body);
    std::string post_string(const std::string &url, const json &body);
    nlohmann::json post_json(const std::string &url, const std::string &body);
    nlohmann::json post_json(const std::string &url, const params &body);
    nlohmann::json post_json(const std::string &url, const json &body);

private:
    std::string params_to_string(const params &params);
    std::string perform();
};

#endif
