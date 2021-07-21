#include <save_switch_common/log.h>
#include "save_storage.h"
#include "save_storage_errors.h"

void save_storage::init() {
    _root = &create_root_entry();
}

const save_storage_entry &save_storage::get_entry(const filepath &path) {
    if (_entry_paths.contains(path))
        return *_entry_paths.at(path);

    log::info(path.string() + " isn't cached, getting...");
    const save_storage_entry *parent = _root;
    for (const auto &part : path) {
        if (_entry_paths.contains(part))
            parent = _entry_paths.at(part);
        else
            parent = &get_parent_entry(*parent, part.filename());
    }

    return *parent;
}

save_storage::entry_list save_storage::get_entries(const filepath &path) {
    return get_parent_entries(get_entry(path));
}

bool save_storage::directory_exists(const filepath &path) {
    try {
        return get_entry(path).type() == save_storage_entry::directory;
    } catch (const path_not_found_error &e) {
        return false;
    }
}

bool save_storage::file_exists(const filepath &path) {
    try {
        return get_entry(path).type() == save_storage_entry::file;
    } catch (const path_not_found_error &e) {
        return false;
    }
}

const save_storage_entry &save_storage::create_directory(const filepath &path) {
    if (!directory_exists(path.parent_path()))
        throw path_not_found_error(path.parent_path());

    if (!directory_exists(path)) {
        log::info(path.string() + " doesn't exist. Creating...");
        create_parent_directory(get_entry(path.parent_path()), path.filename());
    }

    return get_entry(path);
}

const save_storage_entry &save_storage::create_directories(const filepath &path) {
    for (const auto &part : path)
        create_directory(part);
    return get_entry(path);
}

const save_storage_entry &save_storage::create_file(const filepath &path, const std::string &text,
                                                    const save_storage::progress_func &progress_func) {
    return create_file(path, std::vector<uint8_t>(text.begin(), text.end()), progress_func);
}

const save_storage_entry &save_storage::create_file(const filepath &path, const std::vector<uint8_t> &bytes,
                                                    const save_storage::progress_func &progress_func) {
    if (file_exists(path))
        throw path_already_exists_error(path);
    if (!directory_exists(path.parent_path()))
        throw path_not_found_error(path.parent_path());

    save_storage::progress_func f = progress_func != nullptr ? progress_func : [](double p) { };
    return create_parent_file(get_entry(path.parent_path()), path.filename(), bytes, f);
}

bool save_storage::is_cached(const std::string &id) {
    return _entries.contains(id);
}

const save_storage_entry &save_storage::get_cache(const std::string &id) {
    return _entries.at(id);
}

const save_storage_entry &save_storage::cache_entry(const save_storage_entry &entry) {
    log::info("Caching " + entry.path().string());
    if (is_cached(entry.id()))
        return get_cache(entry.id());

    _entries.emplace(entry.id(), entry);
    _entry_paths.emplace(entry.path(), &_entries.at(entry.id()));
    return get_cache(entry.id());
}
