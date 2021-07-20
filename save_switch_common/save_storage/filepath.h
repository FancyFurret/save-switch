#ifndef SAVE_SWITCH_FILEPATH_H
#define SAVE_SWITCH_FILEPATH_H

#include <vector>
#include <string>
#include <iterator>

class filepath {
    const std::string _separator = "/";

    std::vector<std::string> _parts;
    std::string _string;

public:
    class const_iterator {
        const filepath &_path;
        int _part;
    public:
        explicit const_iterator(const filepath &path, int part = 0) : _path(path), _part(part) { }

        const_iterator &operator++() {
            _part++;
            return *this;
        }

        const_iterator operator++(int) { // NOLINT(cert-dcl21-cpp)
            const_iterator ret = *this;
            ++(*this);
            return ret;
        }

        bool operator==(const_iterator other) const {
            return _part == other._part;
        }

        bool operator!=(const_iterator other) const {
            return !(*this == other);
        }

        filepath operator*() {
            return filepath(_path.get_string(_part + 1));
        }
    };

    filepath();
    filepath(const char *str); // NOLINT(google-explicit-constructor)
    filepath(const std::string &str); // NOLINT(google-explicit-constructor)

    [[nodiscard]] std::string filename() const;
    [[nodiscard]] std::string string() const;
    [[nodiscard]] filepath parent_path() const;

    [[nodiscard]] const_iterator begin() const;
    [[nodiscard]] const_iterator end() const;

    bool operator==(const filepath &other) const;
    bool operator!=(const filepath &other) const;
    filepath operator/(char *other) const;
    filepath operator/(const std::string &other) const;

private:
    [[nodiscard]] std::string get_string(int parts) const;
};

namespace std {
    template<>
    struct hash<filepath> {
        std::size_t operator()(const filepath &path) const {
            return hash<string>()(path.string());
        }
    };
}

#endif
