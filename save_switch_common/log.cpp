#include "log.h"
#include <iostream>

#ifdef SWITCH

#include <switch.h>

#endif

void log::info(const std::string &message) {
    std::cout << "[INFO] " << message << std::endl;

#ifdef SWITCH
    consoleUpdate(nullptr);
#endif
}

void log::error(const std::string &message) {
    std::cout << "[ERROR] " << message << std::endl;

#ifdef SWITCH
    consoleUpdate(nullptr);
#endif
}