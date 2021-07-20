#include "http_client.h"
#include <save_switch_common/log.h>

void http_client::init() {
    _curl = curl_easy_init();
    if (!_curl)
        log::error("Could not init curl!");
}

void http_client::set_header(const std::string &name, const std::string &value) {
    std::string header = name + ": " + value;
    _user_headers = curl_slist_append(_user_headers, header.c_str());
}

std::string http_client::get_string(const std::string &url, const params &params) {
    log::info("Sending GET request to " + url);
    curl_easy_reset(_curl);
    curl_easy_setopt(_curl, CURLOPT_URL, (url + "?" + params_to_string(params)).c_str());
    return perform();
}

nlohmann::json http_client::get_json(const std::string &url, const params &params) {
    return nlohmann::json::parse(get_string(url, params));
}

std::string http_client::post_string(const std::string &url, const std::string &body) {
    log::info("Sending POST request to " + url);
    curl_easy_reset(_curl);
    curl_easy_setopt(_curl, CURLOPT_URL, url.c_str());
    curl_easy_setopt(_curl, CURLOPT_POSTFIELDS, body.c_str());
    return perform();
}

std::string http_client::post_string(const std::string &url, const params &body) {
    return post_string(url, params_to_string(body));
}

std::string http_client::post_string(const std::string &url, const json &body) {
    _headers = curl_slist_append(_headers, "Content-Type: application/json");
    return post_string(url, body.dump());
}

nlohmann::json http_client::post_json(const std::string &url, const std::string &body) {
    return nlohmann::json::parse(post_string(url, body));
}

nlohmann::json http_client::post_json(const std::string &url, const params &body) {
    return nlohmann::json::parse(post_string(url, body));
}

nlohmann::json http_client::post_json(const std::string &url, const json &body) {
    return nlohmann::json::parse(post_string(url, body));
}

std::string http_client::params_to_string(const params &params) {
    std::string fields;
    for (const auto &kv : params) {
        if (!fields.empty())
            fields += "&";

        auto escaped = curl_easy_escape(_curl, kv.second.c_str(), (int) kv.second.length());
        fields += kv.first + "=" + escaped;
        curl_free(escaped);
    }
    return fields;
}

std::string http_client::perform() {
    std::string result_body;

    curl_slist *current = _user_headers;
    while (current != nullptr) {
        _headers = curl_slist_append(_headers, current->data);
        current = current->next;
    }

    curl_easy_setopt(_curl, CURLOPT_HTTPHEADER, _headers);
    curl_easy_setopt(_curl, CURLOPT_WRITEDATA, &result_body);
    curl_easy_setopt(_curl, CURLOPT_WRITEFUNCTION, static_cast<size_t (*)(char *, size_t, size_t, void *)>(
            [](char *ptr, size_t size, size_t nmemb, void *result_body) {
                *(static_cast<std::string *>(result_body)) += std::string { ptr, size * nmemb };
                return size * nmemb;
            }
    ));

    _res = curl_easy_perform(_curl);
    curl_slist_free_all(_headers);
    _headers = nullptr;

    if (_res != CURLE_OK) {
        std::string err_string = curl_easy_strerror(_res);
        throw std::runtime_error("Perform failed: " + err_string);
    }

    return result_body;
}
