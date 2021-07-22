#include <switch.h>
#include <save_switch_common/log.h>
#include "config.h"
#include "save_switch.h"

void init() {
    consoleInit(nullptr);
    socketInitializeDefault();
    appletInitialize();
    nsInitialize();
    hidInitialize();
    accountInitialize(AccountServiceType_System);

#ifdef SS_DEBUG
    nxlinkStdio();
#endif
}

void exit() {
    consoleExit(nullptr);
    socketExit();
    appletExit();
    nsExit();
    hidExit();
    accountExit();
}

void run_app() {
    std::cout << "Welcome to SaveSwitch!" << std::endl;

    save_switch ss;
    ss.init();
    ss.update_cloud_switch_info();
}

void wait_for_exit() {
    std::cout << std::endl << "Press + to exit..." << std::endl;

    PadState pad_state;
    padConfigureInput(1, HidNpadStyleSet_NpadStandard);
    padInitializeDefault(&pad_state);

    while (appletMainLoop()) {
        padUpdate(&pad_state);
        u64 down = padGetButtonsDown(&pad_state);

        if (down & HidNpadButton_Plus)
            break;

        consoleUpdate(nullptr);
    }

    exit();
}

int main() {
    init();

    // Run everything in a try-catch so the whole switch doesn't crash...
    try {
        run_app();
    } catch (const std::exception &e) {
        log::error("Uncaught exception:");
        log::error(e.what());
    } catch (...) {
        log::error("Unknown uncaught exception");
    }

    wait_for_exit();
    return 0;
}
