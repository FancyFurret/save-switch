#include "http_client.h"
#include <save_switch_common/log.h>

void http_client::init() {
    _curl = curl_easy_init();
    if (!_curl)
        throw std::runtime_error("Could not init curl!");
}

http_client::~http_client() {
    curl_easy_cleanup(_curl);
}

void http_client::set_auth_header(const std::string &value) {
    _headers["Authorization"] = value;
}

http_request http_client::get(const std::string &url) {
    log::info("Sending GET request to " + url);
    return http_request(this, url, http_request::method::get);
}

http_request http_client::post(const std::string &url) {
    log::info("Sending POST request to " + url);
    return http_request(this, url, http_request::method::post);
}

http_request http_client::put(const std::string &url) {
    log::info("Sending PUT request to " + url);
    return http_request(this, url, http_request::method::put);
}

http_request http_client::patch(const std::string &url) {
    log::info("Sending PATCH request to " + url);
    return http_request(this, url, http_request::method::patch);
}

std::unique_ptr<const http_response> http_client::send(const http_request &request) {
    auto res = std::make_unique<http_response>();

    // Reset
    curl_easy_reset(_curl);
    curl_easy_setopt(_curl, CURLOPT_VERBOSE, true);

    // URL
    if (!request._query.empty())
        curl_easy_setopt(_curl, CURLOPT_URL, (request._url + "?" + get_query_string(request._query)).c_str());
    else
        curl_easy_setopt(_curl, CURLOPT_URL, request._url.c_str());

    // Method
    switch (request._method) {
        case http_request::get:
            break;
        case http_request::post:
            curl_easy_setopt(_curl, CURLOPT_POST, true);
            break;
        case http_request::put:
            curl_easy_setopt(_curl, CURLOPT_PUT, true);
            break;
        case http_request::patch:
            curl_easy_setopt(_curl, CURLOPT_CUSTOMREQUEST, "PATCH");
            break;
    }

    // Headers
    curl_slist *headers = nullptr;
    for (const auto &header : get_header_list(request._headers))
        headers = curl_slist_append(headers, header.c_str());
    for (const auto &header : get_header_list(_headers))
        headers = curl_slist_append(headers, header.c_str());
    curl_easy_setopt(_curl, CURLOPT_HTTPHEADER, headers);

    // Body
    if (request._body != nullptr) {
        curl_easy_setopt(_curl, CURLOPT_INFILESIZE_LARGE, request._body->size());
        curl_easy_setopt(_curl, CURLOPT_POSTFIELDSIZE_LARGE, request._body->size());
        curl_easy_setopt(_curl, CURLOPT_READDATA, request._body.get());
        curl_easy_setopt(_curl, CURLOPT_READFUNCTION, static_cast<size_t (*)(char *, size_t, size_t, void *)>(
                [](char *ptr, size_t size, size_t nmemb, void *result_body) {
                    size_t size_in = size * nmemb;
                    auto *data = (std::vector<uint8_t> *) result_body;
                    size_t size_read = std::min(data->size(), size_in);
                    std::memcpy(ptr, data->data(), size_read);
                    return size_read;
                }
        ));
    }

    // Response Data
    curl_easy_setopt(_curl, CURLOPT_WRITEDATA, res->_data.get());
    curl_easy_setopt(_curl, CURLOPT_WRITEFUNCTION, static_cast<size_t (*)(char *, size_t, size_t, void *)>(
            [](char *ptr, size_t size, size_t nmemb, void *result_body) {
                size_t size_in = size * nmemb;
                auto *data = (std::vector<uint8_t> *) result_body;
                data->reserve(size_in);
                data->insert(data->end(), ptr, ptr + size_in);
                return size_in;
            }
    ));

    // Response Headers
    curl_easy_setopt(_curl, CURLOPT_HEADERDATA, &res->_headers);
    curl_easy_setopt(_curl, CURLOPT_HEADERFUNCTION, static_cast<size_t (*)(char *, size_t, size_t, void *)>(
            [](char *ptr, size_t size, size_t nmemb, void *result_body) {
                size_t size_in = size * nmemb;
                auto *data = (std::unordered_map<std::string, std::string> *) result_body;
                auto str = std::string(ptr, size_in);
                str.erase(std::remove(str.begin(), str.end(), '\n'), str.end());
                str.erase(std::remove(str.begin(), str.end(), '\r'), str.end());

                size_t pos = str.find(": ");
                if (pos != std::string::npos)
                    data->emplace(str.substr(0, pos), str.substr(pos + 2, str.size()));

                return size_in;
            }
    ));

    _res = curl_easy_perform(_curl);
    curl_slist_free_all(headers);

    if (_res != CURLE_OK) {
        std::string err_string = curl_easy_strerror(_res);
        throw std::runtime_error("Perform failed: " + err_string);
    }

    curl_easy_getinfo(_curl, CURLINFO_RESPONSE_CODE, &res->_code);
    return res;
}

std::vector<std::string> http_client::get_header_list(const headers &headers) const {
    std::vector<std::string> list;
    for (const auto &kv : headers)
        list.emplace_back(kv.first + ": " + kv.second);
    return list;
}

std::string http_client::get_query_string(const params &params) const {
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
