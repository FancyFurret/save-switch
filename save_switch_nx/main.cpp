#include <switch.h>
#include <save_switch_common/save_storage/google_drive/google_drive.h>
#include <save_switch_nx/switch/switch_data.h>
#include <save_switch_common/log.h>
#include "config.h"

int run_app() {
    consoleInit(nullptr);
    socketInitializeDefault();
    appletInitialize();
    nsInitialize();
    hidInitialize();
    accountInitialize(AccountServiceType_System);

    printf("Welcome to SaveSwitch!\n");

    printf("Loading save_switch_nx config\n");
    if (!std::filesystem::exists("/switch/SaveSwitch"))
        std::filesystem::create_directory("/switch/SaveSwitch");
    config config(std::filesystem::path("/switch/SaveSwitch/config.json"));

    auto *storage = new google_drive();
    storage->init(&config);

    if (!storage->is_authenticated()) {
        printf("Google Drive is not authenticated!\n");
        storage->authenticate();
        config.save();
        printf("Done!");
    }

    for (const auto &entry : storage->get_entries("SaveSwitch"))
        log::info("Found existing entry: " + entry.get().name());

    printf("Getting SaveSwitch folder\n");
    const auto &folder = storage->get_or_create_folder("SaveSwitch");

    printf("Loading save data from switch...");
    switch_data data;
//    data.load();
    printf("Done!");

    // Main loop
    while (appletMainLoop()) {
        consoleUpdate(nullptr);
    }

    consoleExit(nullptr);
    delete storage;
    return 0;
}

int main() {
    // Run everything in a try-catch so the whole switch doesn't crash...
    try {
        run_app();
    } catch (const std::exception &e) {
        log::error("Uncaught exception!");
        log::error(e.what());
        while (appletMainLoop()) { consoleUpdate(nullptr); }
    }
    catch (...) {
        log::error("Unknown uncaught exception!");
        while (appletMainLoop()) { consoleUpdate(nullptr); }
    }
}
