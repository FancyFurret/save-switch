#ifndef SAVE_SWITCH_SAVE_STORAGE_ENTRY_H
#define SAVE_SWITCH_SAVE_STORAGE_ENTRY_H

#include <filesystem>
#include "filepath.h"

class save_storage_entry {
public:
    enum type {
        file,
        directory
    };

private:
    std::string _id;
    std::string _name;
    filepath _path;
    type _type;

public:
    save_storage_entry(const std::string &id, const filepath &path, type type)
            : _id(id),
              _name(path.filename()),
              _path(path),
              _type(type) { }

    [[nodiscard]] const std::string &id() const {
        return _id;
    }

    [[nodiscard]] const std::string &name() const {
        return _name;
    }

    [[nodiscard]] type type() const {
        return _type;
    }

    [[nodiscard]] filepath path() const {
        return _path;
    }
};

#endif
