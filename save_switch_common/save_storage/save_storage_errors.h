#ifndef SAVE_SWITCH_SAVE_STORAGE_ERRORS_H
#define SAVE_SWITCH_SAVE_STORAGE_ERRORS_H

#include <stdexcept>
#include <filesystem>

class path_not_found_error : public std::runtime_error {
public:
    path_not_found_error(const save_storage_entry &parent, const std::string &name)
            : std::runtime_error((parent.path() / name).string() + " does not exist!") { }

    explicit path_not_found_error(const filepath &filepath)
            : std::runtime_error(filepath.string() + " does not exist!") { }
};

class path_already_exists_error : public std::runtime_error {
public:
    explicit path_already_exists_error(const filepath &path)
            : std::runtime_error(path.string() + " already exists!") { }
};

#endif
