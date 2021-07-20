#ifndef SAVE_SWITCH_SWITCH_SAVE_H
#define SAVE_SWITCH_SWITCH_SAVE_H

#include "switch_app.h"
#include <switch.h>

class switch_save {
    uint64_t _id;
    const switch_app &_app;

public:
    switch_save(switch_app *app, FsSaveDataInfo *info) : _id(info->save_data_id), _app(*app) { }

    [[nodiscard]] uint64_t id() const {
        return _id;
    }

    [[nodiscard]] const switch_app &app() const {
        return _app;
    }

};

#endif
