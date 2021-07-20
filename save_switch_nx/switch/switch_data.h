#ifndef SAVE_SWITCH_SWITCH_DATA_H
#define SAVE_SWITCH_SWITCH_DATA_H

#include <unordered_map>
#include <string>
#include "switch_user.h"
#include "switch_app.h"
#include "switch_save.h"

class switch_data {
    std::unordered_map<std::string, switch_user> _users;
    std::unordered_map<uint64_t, switch_app> _apps;
    std::unordered_map<uint64_t, switch_save> _saves;

public:
    void load();

    const std::unordered_map<std::string, switch_user> &users();
    const std::unordered_map<uint64_t, switch_app> &apps();
    const std::unordered_map<uint64_t, switch_save> &saves();
};

#endif
