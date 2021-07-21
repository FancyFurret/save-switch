#ifndef SAVE_SWITCH_SAVE_STORAGE_H
#define SAVE_SWITCH_SAVE_STORAGE_H

#include <string>
#include <vector>
#include <unordered_map>
#include <functional>
#include <save_switch_common/save_storage/google_drive/google_drive_cache.h>
#include "save_storage_entry.h"

class save_storage {
    std::unordered_map<std::string, save_storage_entry> _entries;
    std::unordered_map<filepath, const save_storage_entry *> _entry_paths;
    const save_storage_entry *_root = nullptr;

public:
    typedef std::vector<std::reference_wrapper<const save_storage_entry>> entry_list;
    typedef std::function<void(double progress)> progress_func;

    virtual void init();

    const save_storage_entry &get_entry(const filepath &path);
    entry_list get_entries(const filepath &path);
    bool directory_exists(const filepath &path);
    bool file_exists(const filepath &path);
    const save_storage_entry &create_directory(const filepath &path);
    const save_storage_entry &create_directories(const filepath &path);
    const save_storage_entry &create_file(
            const filepath &path, const std::string &text, const progress_func &progress_func = nullptr);
    const save_storage_entry &create_file(
            const filepath &path, const std::vector<uint8_t> &bytes, const progress_func &progress_func = nullptr);

    virtual ~save_storage() = default;

protected:
    virtual const save_storage_entry &get_parent_entry(const save_storage_entry &parent, const std::string &name) = 0;
    virtual entry_list get_parent_entries(const save_storage_entry &parent) = 0;
    virtual const save_storage_entry &create_parent_directory(
            const save_storage_entry &parent, const std::string &name) = 0;
    virtual const save_storage_entry &create_parent_file(
            const save_storage_entry &parent, const std::string &name, const std::vector<uint8_t> &bytes,
            const progress_func &progress_func) = 0;
    virtual const save_storage_entry &create_root_entry() = 0;

    bool is_cached(const std::string &id);
    const save_storage_entry &get_cache(const std::string &id);
    const save_storage_entry &cache_entry(const save_storage_entry &entry);
};

#endif
