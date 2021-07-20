#include "google_drive.h"
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

    nlohmann::json res = _http_client.get_json("https://www.googleapis.com/drive/v3/files", params {
            { "q",        q.str() },
            { "spaces",   "drive" },
            { "pageSize", "1" },
    });
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

    nlohmann::json res = _http_client.get_json("https://www.googleapis.com/drive/v3/files", params {
            { "q",        q.str() },
            { "spaces",   "drive" },
            { "pageSize", "100" },
    });
    ensure_ok(res);

    entry_list entries;
    for (const auto &file : res["files"])
        entries.push_back(parse_json_entry(parent, file));

    return entries;
}

const save_storage_entry &
google_drive::create_parent_folder(const save_storage_entry &parent, const std::string &name) {
    nlohmann::json res = _http_client.post_json("https://www.googleapis.com/drive/v3/files", json {
            { "parents",  { parent.id() }},
            { "name",     name },
            { "mimeType", "application/vnd.google-apps.folder" }
    });
    ensure_ok(res);
    return parse_json_entry(parent, res);
}

const save_storage_entry &google_drive::create_root_entry() {
    return cache_entry(save_storage_entry("root", "", save_storage_entry::directory));
}

bool google_drive::is_authenticated() {
    return verify_access_token();
}

bool google_drive::authenticate() {
    nlohmann::json res = _http_client.post_json("https://oauth2.googleapis.com/device/code", params {
            { "client_id", GOOGLE_CLIENT_ID },
            { "scope", "https://www.googleapis.com/auth/drive.file" }
    });

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
    nlohmann::json res = _http_client.post_json("https://www.googleapis.com/oauth2/v1/tokeninfo", params {
            { "access_token", _access_token }
    });

    if (res.contains("error")) {
        res = _http_client.post_json("https://oauth2.googleapis.com/token", params {
                { "client_id",     GOOGLE_CLIENT_ID },
                { "client_secret", GOOGLE_CLIENT_SECRET },
                { "grant_type",    "refresh_token" },
                { "refresh_token", _cache->google_refresh_token() }
        });

        if (res.contains("error")) {
            log::error("Could not get new access token! Resetting refresh token...");
            return false;
        }

        _access_token = res["access_token"];
    }

    _http_client.set_header("Authorization", "Bearer " + _access_token);
    return true;
}

bool google_drive::poll_auth_code(const std::string &device_code) {
    log::info("Polling auth code");
    nlohmann::json res = _http_client.post_json("https://oauth2.googleapis.com/token", params {
            { "client_id",     GOOGLE_CLIENT_ID },
            { "client_secret", GOOGLE_CLIENT_SECRET },
            { "device_code", device_code },
            { "grant_type",  "urn:ietf:params:oauth:grant-type:device_code" }
    });

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