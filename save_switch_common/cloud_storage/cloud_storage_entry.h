#ifndef SAVE_SWITCH_CLOUD_STORAGE_ENTRY_H
#define SAVE_SWITCH_CLOUD_STORAGE_ENTRY_H

#include <filesystem>
#include "filepath.h"

class cloud_storage_entry {
public:
    enum entry_type {
        file,
        directory
    };

    enum entry_hash_type {
        none,
        md_5
    };

private:
    std::string _id;
    std::string _name;
    filepath _path;
    entry_type _type;
    std::string _hash;
    entry_hash_type _hash_type;

public:
    cloud_storage_entry(const std::string &id, const filepath &path, entry_type type)
            : _id(id),
              _name(path.filename()),
              _path(path),
              _type(type),
              _hash_type(none) { }

    cloud_storage_entry(const std::string &id, const filepath &path, entry_type type, const std::string &hash,
                        entry_hash_type hash_type)
            : _id(id),
              _name(path.filename()),
              _path(path),
              _type(type),
              _hash(hash),
              _hash_type(hash_type) { }

    [[nodiscard]] const std::string &id() const {
        return _id;
    }

    [[nodiscard]] const std::string &name() const {
        return _name;
    }

    [[nodiscard]] entry_type type() const {
        return _type;
    }

    [[nodiscard]] filepath path() const {
        return _path;
    }

    [[nodiscard]] std::string hash() const {
        return _hash;
    }

    [[nodiscard]] entry_hash_type hash_type() const {
        return _hash_type;
    }
};

#endif
