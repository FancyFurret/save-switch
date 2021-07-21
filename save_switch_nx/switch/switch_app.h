#ifndef SAVE_SWITCH_SWITCH_APP_H
#define SAVE_SWITCH_SWITCH_APP_H

#include <stdexcept>
#include <switch.h>

class switch_app {
    std::string _id;
    std::string _name;

public:
    explicit switch_app(FsSaveDataInfo *info) {
        _id = utils::to_hex_string(info->application_id);

        NacpLanguageEntry *language_entry;
        static NsApplicationControlData control;

        size_t outsize;
        Result res = nsGetApplicationControlData(NsApplicationControlSource_Storage, info->application_id, &control,
                                                 sizeof(NsApplicationControlData), &outsize);
        if (R_FAILED(res))
            throw std::runtime_error("Could not get app control data!");

        res = nacpGetLanguageEntry(&control.nacp, &language_entry);
        if (R_FAILED(res))
            throw std::runtime_error("Could not get app language entry!");

        _name = language_entry->name;
    }

    [[nodiscard]] std::string id() const {
        return _id;
    }

    [[nodiscard]] const std::string &name() const {
        return _name;
    }
};

#endif
