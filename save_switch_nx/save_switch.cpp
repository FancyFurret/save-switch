#include "save_switch.h"
#include <save_switch_common/log.h>
#include <save_switch_common/cloud_storage/google_drive/google_drive.h>

void save_switch::init() {
    load_config();
    load_storage();
    load_data();

    _storage->create_directories("/SaveSwitch");
}

void save_switch::update_cloud_switch_info() {
    std::stringstream ss;

    log::info("Updating switch info in cloud");

    ss << "Users" << std::endl;
    ss << "=====" << std::endl;
    for (const auto &user : _data->users())
        ss << user.second.id() << " - " << user.second.name() << std::endl;
    ss << std::endl << std::endl;

    ss << "Titles" << std::endl;
    ss << "======" << std::endl;
    for (const auto &app : _data->apps())
        ss << app.second.id() << " - " << app.second.name() << std::endl;
    ss << std::endl << std::endl << std::endl;

    // TODO: Only upload if hash is different
    _storage->create_file("/SaveSwitch/SwitchIds.txt", ss.str(), true);
}

void save_switch::load_config() {
    log::info("Loading SaveSwitchNx config");
    if (!std::filesystem::exists("/switch/SaveSwitch"))
        std::filesystem::create_directories("/switch/SaveSwitch");

    _config = std::make_unique<config>(std::filesystem::path("/switch/SaveSwitch/config.json"));
}

void save_switch::load_storage() {
    // TODO: Support multiple backends?
    log::info("Loading cloud storage");
    _storage = std::make_unique<google_drive>(_config.get());
    if (!_storage->is_authenticated()) {
        log::info("Google Drive is not authenticated");
        _storage->authenticate();
        _config->save();
        log::info("Authentication successful");
    }
}

void save_switch::load_data() {
    log::info("Loading data from switch");
    _data = std::make_unique<switch_data>();
    _data->load();
}
