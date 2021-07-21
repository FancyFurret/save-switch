#ifndef SAVE_SWITCH_SWITCH_SAVE_H
#define SAVE_SWITCH_SWITCH_SAVE_H

#include "switch_app.h"
#include <switch.h>

class switch_save {
    std::string _id;
    const switch_app &_app;

public:
    switch_save(switch_app *app, FsSaveDataInfo *info) : _id(utils::to_hex_string(info->save_data_id)), _app(*app) { }

    [[nodiscard]] std::string id() const {
        return _id;
    }

    [[nodiscard]] const switch_app &app() const {
        return _app;
    }

};

#endif
