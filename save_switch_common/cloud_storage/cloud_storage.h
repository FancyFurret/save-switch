#ifndef SAVE_SWITCH_CLOUD_STORAGE_H
#define SAVE_SWITCH_CLOUD_STORAGE_H

#include <string>
#include <vector>
#include <unordered_map>
#include <functional>
#include <save_switch_common/cloud_storage/google_drive/google_drive_cache.h>
#include <save_switch_common/http_client/http_types.h>
#include "cloud_storage_entry.h"

class cloud_storage {
    std::unordered_map<std::string, cloud_storage_entry> _entries;
    std::unordered_map<filepath, const cloud_storage_entry *> _entry_paths;
    const cloud_storage_entry *_root = nullptr;

public:
    typedef std::vector<std::reference_wrapper<const cloud_storage_entry>> entry_list;
    typedef std::function<void(double progress)> progress_func;

    void init();

    virtual bool is_authenticated() = 0;
    virtual void authenticate() = 0;

    const cloud_storage_entry &get_entry(const filepath &path);
    entry_list get_entries(const filepath &path);
    bool directory_exists(const filepath &path);
    bool file_exists(const filepath &path);
    const cloud_storage_entry &create_directory(const filepath &path);
    const cloud_storage_entry &create_directories(const filepath &path);
    const cloud_storage_entry &create_file(const filepath &path, const std::string &text,
                                           bool overwrite = false, const progress_func &progress_func = nullptr);
    const cloud_storage_entry &create_file(const filepath &path, byte_array bytes,
                                           bool overwrite = false, const progress_func &progress_func = nullptr);

    virtual ~cloud_storage() = default;

protected:
    virtual const cloud_storage_entry &get_parent_entry(const cloud_storage_entry &parent, const std::string &name) = 0;
    virtual entry_list get_parent_entries(const cloud_storage_entry &parent) = 0;
    virtual const cloud_storage_entry &create_parent_directory(
            const cloud_storage_entry &parent, const std::string &name) = 0;
    virtual const cloud_storage_entry &create_parent_file(
            const cloud_storage_entry &parent, const std::string &name,
            byte_array bytes, const progress_func &progress_func) = 0;
    virtual const cloud_storage_entry &update_parent_file(
            const cloud_storage_entry &entry, byte_array bytes, const progress_func &progress_func) = 0;
    virtual const cloud_storage_entry &create_root_entry() = 0;

    bool is_cached(const std::string &id);
    const cloud_storage_entry &get_cache(const std::string &id);
    const cloud_storage_entry &cache_entry(const cloud_storage_entry &entry);
};

#endif
