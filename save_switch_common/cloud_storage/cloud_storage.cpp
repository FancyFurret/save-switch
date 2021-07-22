#include <save_switch_common/log.h>
#include <fmt/format.h>
#include "cloud_storage.h"
#include "cloud_storage_errors.h"

void cloud_storage::init() {
    _root = &create_root_entry();
}

const cloud_storage_entry &cloud_storage::get_entry(const filepath &path) {
    if (_entry_paths.contains(path))
        return *_entry_paths.at(path);

    log::info(path.string() + " isn't cached, getting...");
    const cloud_storage_entry *parent = _root;
    for (const auto &part : path) {
        if (_entry_paths.contains(part))
            parent = _entry_paths.at(part);
        else
            parent = &get_parent_entry(*parent, part.filename());
    }

    return *parent;
}

cloud_storage::entry_list cloud_storage::get_entries(const filepath &path) {
    return get_parent_entries(get_entry(path));
}

bool cloud_storage::directory_exists(const filepath &path) {
    try {
        return get_entry(path).type() == cloud_storage_entry::directory;
    } catch (const path_not_found_error &e) {
        return false;
    }
}

bool cloud_storage::file_exists(const filepath &path) {
    try {
        return get_entry(path).type() == cloud_storage_entry::file;
    } catch (const path_not_found_error &e) {
        return false;
    }
}

const cloud_storage_entry &cloud_storage::create_directory(const filepath &path) {
    if (!directory_exists(path.parent_path()))
        throw path_not_found_error(path.parent_path());

    if (!directory_exists(path)) {
        log::info(path.string() + " doesn't exist. Creating...");
        create_parent_directory(get_entry(path.parent_path()), path.filename());
    }

    return get_entry(path);
}

const cloud_storage_entry &cloud_storage::create_directories(const filepath &path) {
    for (const auto &part : path)
        create_directory(part);
    return get_entry(path);
}

const cloud_storage_entry &cloud_storage::create_file(const filepath &path, const std::string &text, bool overwrite,
                                                      const cloud_storage::progress_func &progress_func) {
    return create_file(path, std::make_unique<const std::vector<uint8_t>>(text.begin(), text.end()),
                       overwrite, progress_func);
}

const cloud_storage_entry &cloud_storage::create_file(const filepath &path,
                                                      byte_array bytes, bool overwrite,
                                                      const cloud_storage::progress_func &progress_func) {
    if (!directory_exists(path.parent_path()))
        throw path_not_found_error(path.parent_path());
    if (file_exists(path) && !overwrite)
        throw path_already_exists_error(path);

    cloud_storage::progress_func f = progress_func != nullptr ? progress_func : [](double p) { };
    if (!file_exists(path))
        return create_parent_file(get_entry(path.parent_path()), path.filename(), std::move(bytes), f);

    const cloud_storage_entry &entry = get_entry(path);
    if (entry.hash().empty() || entry.hash_type() == cloud_storage_entry::none)
        log::error("File does not have hash in cloud storage! Contents may be re-uploaded for " + path.string());

    // Get hash of bytes to be uploaded
    std::string hash;
    switch (entry.hash_type()) {
        case cloud_storage_entry::none:
            break;
        case cloud_storage_entry::md_5:
            hash = _md_5_hasher(bytes->data(), bytes->size());
            break;
    }

    if (hash == entry.hash()) {
        log::info("File in cloud storage has same hash. Won't upload again");
        f(1);
        return entry;
    }

    log::info(fmt::format("File in cloud storage has different hash ({} vs {}). Uploading", entry.hash(), hash));
    return update_parent_file(get_entry(path), std::move(bytes), f);
}

bool cloud_storage::is_cached(const std::string &id) {
    return _entries.contains(id);
}

const cloud_storage_entry &cloud_storage::get_cache(const std::string &id) {
    return _entries.at(id);
}

const cloud_storage_entry &cloud_storage::cache_entry(const cloud_storage_entry &entry) {
    log::info("Caching " + entry.path().string());
    _entries.emplace(entry.id(), entry);
    _entry_paths.emplace(entry.path(), &_entries.at(entry.id()));
    return get_cache(entry.id());
}
