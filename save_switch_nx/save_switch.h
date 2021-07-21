#ifndef SAVE_SWITCH_SAVE_SWITCH_H
#define SAVE_SWITCH_SAVE_SWITCH_H

#include <save_switch_common/cloud_storage/cloud_storage.h>
#include <save_switch_nx/switch/switch_data.h>
#include "config.h"

class save_switch {
    std::unique_ptr<config> _config;
    std::unique_ptr<cloud_storage> _storage;
    std::unique_ptr<switch_data> _data;

public:
    void init();
    void update_cloud_switch_info();

private:
    void load_config();
    void load_storage();
    void load_data();
};

#endif
