#ifndef SAVE_SWITCH_GOOGLE_DRIVE_H
#define SAVE_SWITCH_GOOGLE_DRIVE_H

#include <string>
#include <save_switch_common/secrets.h>
#include <save_switch_common/cloud_storage/cloud_storage.h>
#include <save_switch_common/http_client/http_client.h>

#if !defined(GOOGLE_CLIENT_ID) or !defined(GOOGLE_CLIENT_SECRET)
#error You must define GOOGLE_CLIENT_ID and GOOGLE_CLIENT_SECRET in save_switch_common/secrets.h in order to build!
#endif

class google_drive : public cloud_storage {
    const std::string _file_fields = "files(id,name,mimeType,md5Checksum)";

    http_client _http_client;
    std::string _access_token;
    google_drive_cache *_cache;

public:
    explicit google_drive(google_drive_cache *cache);

    bool is_authenticated() override;
    void authenticate() override;

protected:
    const cloud_storage_entry &get_parent_entry(const cloud_storage_entry &parent, const std::string &name) override;
    entry_list get_parent_entries(const cloud_storage_entry &parent) override;
    const cloud_storage_entry &create_parent_directory(
            const cloud_storage_entry &parent, const std::string &name) override;
    const cloud_storage_entry &create_parent_file(
            const cloud_storage_entry &parent, const std::string &name,
            byte_array bytes, const progress_func &progress_func) override;
    const cloud_storage_entry &update_parent_file(
            const cloud_storage_entry &entry, byte_array bytes, const progress_func &progress_func) override;
    const cloud_storage_entry &create_root_entry() override;

private:
    bool verify_access_token();
    bool poll_auth_code(const std::string &device_code);
    void ensure_ok(const nlohmann::json &res);
    std::unique_ptr<const http_response> resumable_chunk_upload(const std::string &location, byte_array bytes,
                                                                const progress_func &progress_func);
    const cloud_storage_entry &parse_json_entry(const cloud_storage_entry &parent, const nlohmann::json &entry);
};

#endif
