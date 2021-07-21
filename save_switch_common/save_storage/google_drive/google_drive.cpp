#include "google_drive.h"
#include <fmt/format.h>
#include <save_switch_common/save_storage/save_storage_errors.h>
#include <save_switch_common/log.h>

void google_drive::init(google_drive_cache *cache) {
    log::info("Initializing Google Drive");
    _http_client.init();
    this->_cache = cache;
    save_storage::init();
}

const save_storage_entry &google_drive::get_parent_entry(const save_storage_entry &parent, const std::string &name) {
    std::stringstream q;
    q << "name='" << name << "' and ";
    q << "'" << parent.id() << "' in parents and ";
    q << "trashed=false";

    nlohmann::json res = _http_client.get("https://www.googleapis.com/drive/v3/files")
            .set_query(params {
                    { "q",        q.str() },
                    { "spaces",   "drive" },
                    { "pageSize", "1" },
            }).send()->json();
    ensure_ok(res);

    if (res["files"].size() != 1)
        throw path_not_found_error(parent, name);

    nlohmann::json file = res["files"][0];
    return parse_json_entry(parent, file);
}

google_drive::entry_list google_drive::get_parent_entries(const save_storage_entry &parent) {
    std::stringstream q;
    q << "'" << parent.id() << "' in parents and ";
    q << "trashed=false";

    nlohmann::json res = _http_client.get("https://www.googleapis.com/drive/v3/files")
            .set_query(params {
                    { "q",        q.str() },
                    { "spaces",   "drive" },
                    { "pageSize", "100" },
            }).send()->json();
    ensure_ok(res);

    entry_list entries;
    for (const auto &file : res["files"])
        entries.push_back(parse_json_entry(parent, file));

    return entries;
}

const save_storage_entry &google_drive::create_parent_directory(const save_storage_entry &parent,
                                                                const std::string &name) {
    nlohmann::json res = _http_client.post("https://www.googleapis.com/drive/v3/files")
            .set_body(json {
                    { "parents",  { parent.id() }},
                    { "name",     name },
                    { "mimeType", "application/vnd.google-apps.folder" }
            }).send()->json();
    ensure_ok(res);
    return parse_json_entry(parent, res);
}

const save_storage_entry &google_drive::create_parent_file(const save_storage_entry &parent, const std::string &name,
                                                           const std::vector<uint8_t> &bytes,
                                                           const save_storage::progress_func &progress_func) {
    auto res = _http_client.post("https://www.googleapis.com/upload/drive/v3/files")
            .set_header("X-Upload-Content-Type", "application/octet-stream")
            .set_header("X-Upload-Content-Length", std::to_string(bytes.size()))
            .set_query(params {
                    { "uploadType", "resumable" }
            })
            .set_body(json {
                    { "parents", { parent.id() }},
                    { "name",    name }
            }).send();

    if (!res->has_header("Location"))
        throw std::runtime_error("Could not start file upload, no location header!");

    std::string location = res->get_header("Location");

    static const size_t chunk_size = 256 * 1024;
    size_t total_length = bytes.size();

    for (size_t pos = 0; pos < total_length; pos += chunk_size) {
        size_t content_length = (pos + chunk_size) <= total_length ? chunk_size : total_length - pos;
        std::string range = fmt::format("bytes {}-{}/{}", pos, pos + content_length - 1, total_length);
        auto chunk = std::make_unique<std::vector<uint8_t>>(bytes.begin() + (long) pos,
                                                            bytes.begin() + (long) pos + (long) content_length);

        log::info("Uploading range " + range);
        res = _http_client.put(location)
                .set_header("Content-Length", std::to_string(content_length))
                .set_header("Content-Range", range)
                .set_body(std::move(chunk))
                .send();

        if (!res->ok())
            throw std::runtime_error("Could not upload part of file! " + res->string());

        progress_func((double) ((long double) (pos + content_length) / (long double) total_length));
    }

    return parse_json_entry(parent, res->json());
}

const save_storage_entry &google_drive::create_root_entry() {
    return cache_entry(save_storage_entry("root", "", save_storage_entry::directory));
}

bool google_drive::is_authenticated() {
    return verify_access_token();
}

bool google_drive::authenticate() {
    nlohmann::json res = _http_client.post("https://oauth2.googleapis.com/device/code")
            .set_body(params {
                    { "client_id", GOOGLE_CLIENT_ID },
                    { "scope", "https://www.googleapis.com/auth/drive.file" }
            }).send()->json();

    std::string device_code = res["device_code"];
    std::string code = res["user_code"];
    std::string url = res["verification_url"];
    int interval = res["interval"];
    int expires_in = res["expires_in"];

    log::info("You need to connect you Google Account to SaveSwitch to get started!");
    log::info("Please go to " + url + " and enter the following code:");
    log::info("Code: " + code);

    time_t start, now;
    time(&start);
    time(&now);

    double diff;
    int checks = 0;
    while ((diff = difftime(now, start)) < expires_in) {
        sleep(1);
        time(&now);

        if (diff < checks * interval)
            continue;

        checks++;
        if (!poll_auth_code(device_code))
            continue;

        log::info("Successfully authed!");
        log::info("Access Token: " + _access_token);
        log::info("Refresh Token: " + _cache->google_refresh_token());

        if (verify_access_token())
            log::info("Verified token!");
        return true;
    }

    log::info("Code has expired!");
    return false;
}

bool google_drive::verify_access_token() {
    nlohmann::json res = _http_client.post("https://www.googleapis.com/oauth2/v1/tokeninfo")
            .set_body(params {
                    { "access_token", _access_token }
            }).send()->json();

    if (res.contains("error")) {
        res = _http_client.post("https://oauth2.googleapis.com/token")
                .set_body(params {
                        { "client_id",     GOOGLE_CLIENT_ID },
                        { "client_secret", GOOGLE_CLIENT_SECRET },
                        { "grant_type",    "refresh_token" },
                        { "refresh_token", _cache->google_refresh_token() }
                }).send()->json();

        if (res.contains("error")) {
            log::error("Could not get new access token! Resetting refresh token...");
            return false;
        }

        _access_token = res["access_token"];
    }

    _http_client.set_auth_header("Bearer " + _access_token);
    return true;
}

bool google_drive::poll_auth_code(const std::string &device_code) {
    log::info("Polling auth code");
    nlohmann::json res = _http_client.post("https://oauth2.googleapis.com/token")
            .set_body(params {
                    { "client_id",     GOOGLE_CLIENT_ID },
                    { "client_secret", GOOGLE_CLIENT_SECRET },
                    { "device_code", device_code },
                    { "grant_type",  "urn:ietf:params:oauth:grant-type:device_code" }
            }).send()->json();

    if (res.contains("error"))
        return false;

    _access_token = res["access_token"];
    _cache->set_google_refresh_token(res["refresh_token"]);
    return true;
}

void google_drive::ensure_ok(const json &res) {
    if (res.contains("error")) {
        log::error("Error returned from Google Drive api: " + res.dump());
        throw std::runtime_error("Error returned from Google Drive api: " + res.dump());
    }
}

const save_storage_entry &google_drive::parse_json_entry(const save_storage_entry &parent, const json &entry) {
    return cache_entry(save_storage_entry(
            entry["id"],
            parent.path() / entry["name"],
            entry["mimeType"] == "application/vnd.google-apps.folder" ? save_storage_entry::directory
                                                                      : save_storage_entry::file));
}
