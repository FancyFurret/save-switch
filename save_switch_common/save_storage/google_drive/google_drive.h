#ifndef SAVE_SWITCH_GOOGLE_DRIVE_H
#define SAVE_SWITCH_GOOGLE_DRIVE_H

#include <string>
#include <save_switch_common/secrets.h>
#include <save_switch_common/save_storage/save_storage.h>
#include <save_switch_common/http_client/http_client.h>

#if !defined(GOOGLE_CLIENT_ID) or !defined(GOOGLE_CLIENT_SECRET)
#error You must define GOOGLE_CLIENT_ID and GOOGLE_CLIENT_SECRET in save_switch_common/secrets.h in order to build!
#endif

class google_drive : public save_storage {
    http_client _http_client;
    std::string _access_token;
    google_drive_cache *_cache;

public:
    void init(google_drive_cache *cache);
    bool is_authenticated();
    bool authenticate();

protected:
    const save_storage_entry &get_parent_entry(const save_storage_entry &parent, const std::string &name) override;
    entry_list get_parent_entries(const save_storage_entry &parent) override;
    const save_storage_entry &create_parent_folder(const save_storage_entry &parent, const std::string &name) override;
    const save_storage_entry &create_root_entry() override;

private:
    bool verify_access_token();
    bool poll_auth_code(const std::string &device_code);
    void ensure_ok(const nlohmann::json &res);
    const save_storage_entry &parse_json_entry(const save_storage_entry &parent, const nlohmann::json &entry);
};

#endif
