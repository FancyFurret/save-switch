#ifndef SAVE_SWITCH_CONFIG_H
#define SAVE_SWITCH_CONFIG_H

#define CEREAL_CONFIG_PUBLIC
#define CEREAL_CONFIG_OVERRIDE
#define CEREAL_CONFIG_PROP_SET_PUBLIC

#include <cereal/cereal_json.h>
#include <save_switch_common/cloud_storage/google_drive/google_drive_cache.h>

class config : public google_drive_cache {
    CEREAL_BEGIN(config)
    CEREAL_PROP(google_refresh_token, std::string)
};

#endif
