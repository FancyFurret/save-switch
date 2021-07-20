#ifndef SAVE_SWITCH_SWITCH_USER_H
#define SAVE_SWITCH_SWITCH_USER_H

#include <save_switch_common/utils.h>
#include "switch_save.h"

class switch_user {
    std::string _id;
    std::string _name;
    std::unordered_map<uint64_t, const switch_save &> _saves { };

public:
    explicit switch_user(AccountUid account_id) {
        _id = uid_to_string(account_id);

        AccountProfile profile;
        AccountProfileBase base;

        Result res = accountGetProfile(&profile, account_id);
        if (R_FAILED(res))
            throw std::runtime_error("Could not get account profile!");

        res = accountProfileGet(&profile, nullptr, &base);
        if (R_FAILED(res))
            throw std::runtime_error("Could not account profile get!");

        _name = base.nickname;

        accountProfileClose(&profile);
    }

    [[nodiscard]] const std::string &id() const {
        return _id;
    }

    [[nodiscard]] const std::string &name() const {
        return _name;
    }

    [[nodiscard]] const std::unordered_map<uint64_t, const switch_save &> &saves() const {
        return _saves;
    }

    void add_save(switch_save *save) {
        _saves.emplace(save->app().id(), *save);
    }

    static std::string uid_to_string(AccountUid id) {
        return utils::to_hex_string(id.uid[0]) + utils::to_hex_string(id.uid[1]);
    }
};

#endif
