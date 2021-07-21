#include "switch_data.h"

void switch_data::load() {
    FsSaveDataInfoReader reader;
    FsSaveDataInfo info;
    s64 entries = 0;

    Result res = fsOpenSaveDataInfoReader(&reader, FsSaveDataSpaceId_User);
    if (R_FAILED(res))
        throw std::runtime_error("Could not open save data reader!");

    while (true) {
        res = fsSaveDataInfoReaderRead(&reader, &info, 1, &entries);
        if (R_FAILED(res))
            throw std::runtime_error("Could not read save data!");

        if (entries == 0)
            break;

        if (info.save_data_type != FsSaveDataType_Account)
            continue;

        std::string tid = utils::to_hex_string(info.application_id);
        std::string sid = utils::to_hex_string(info.save_data_id);
        std::string uid = switch_user::uid_to_string(info.uid);

        if (!_saves.contains(tid))
            _apps.emplace(tid, switch_app(&info));
        if (!_users.contains(uid))
            _users.emplace(uid, switch_user(info.uid));

        switch_app *app = &_apps.at(tid);
        switch_user *user = &_users.at(uid);

        _saves.emplace(sid, switch_save(app, &info));
        switch_save *save = &_saves.at(sid);

        user->add_save(save);
    }

    fsSaveDataInfoReaderClose(&reader);
}

const std::map<std::string, switch_user> &switch_data::users() {
    return _users;
}

const std::map<std::string, switch_app> &switch_data::apps() {
    return _apps;
}

const std::map<std::string, switch_save> &switch_data::saves() {
    return _saves;
}

