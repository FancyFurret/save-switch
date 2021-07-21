#ifndef SAVE_SWITCH_SWITCH_DATA_H
#define SAVE_SWITCH_SWITCH_DATA_H

#include <map>
#include <string>
#include <vector>
#include "switch_user.h"
#include "switch_app.h"
#include "switch_save.h"

class switch_data {
    // TODO: Order these?
    std::map<std::string, switch_user> _users;
    std::map<std::string, switch_app> _apps;
    std::map<std::string, switch_save> _saves;

public:
    void load();

    const std::map<std::string, switch_user> &users();
    const std::map<std::string, switch_app> &apps();
    const std::map<std::string, switch_save> &saves();
};

#endif
