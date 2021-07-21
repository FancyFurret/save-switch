#include <sstream>
#include "filepath.h"

filepath::filepath() : filepath(_separator) { }

filepath::filepath(const char *str) : filepath(std::string(str)) { }

filepath::filepath(const std::string &str) {
    uint64_t start = 0;
    uint64_t end = str.find(_separator);
    while (end != std::string::npos) {
        if (end - start > 0)
            _parts.push_back(str.substr(start, end - start));
        start = end + _separator.length();
        end = str.find(_separator, start);
    }

    _parts.push_back(str.substr(start, end));

    _string = get_string((int) _parts.size());
}

std::string filepath::filename() const {
    return _parts.back();
}

std::string filepath::string() const {
    return _string;
}

filepath filepath::parent_path() const {
    if (_parts.empty())
        return filename();
    return filepath(get_string((int) _parts.size() - 1));
}

filepath::const_iterator filepath::begin() const {
    return const_iterator(*this);
}

filepath::const_iterator filepath::end() const {
    return const_iterator(*this, (int) _parts.size());
}

bool filepath::operator==(const filepath &other) const {
    return _string == other.string();
}

bool filepath::operator!=(const filepath &other) const {
    return !(*this == other);
}

filepath filepath::operator/(char *other) const {
    return operator/(std::string(other));
}

filepath filepath::operator/(const std::string &other) const {
    return filepath(string() + _separator + other);
}

std::string filepath::get_string(int parts) const {
    if (parts == 0)
        return _separator;

    std::stringstream ss;
    for (int i = 0; i < parts; i++)
        ss << _separator << _parts[i];
    return ss.str();
}