#ifndef SAVE_SWITCH_GOOGLE_DRIVE_CACHE_H
#define SAVE_SWITCH_GOOGLE_DRIVE_CACHE_H

#include <string>

class google_drive_cache {
public:
    // TODO: Add CamelCase option to cereal key format settings
    [[nodiscard]] virtual const std::string &google_refresh_token() const = 0;
    virtual void set_google_refresh_token(const std::string &value) = 0;
};

#endif
