#include "log.h"
#include "terminal.h"
#include <iostream>

#ifdef SWITCH

#include <switch.h>

#endif

void log::info(const std::string &message) {
    std::cout << "[" << term::color::cyan << "INFO" << term::reset << "] " << message << std::endl;

#ifdef SWITCH
    consoleUpdate(nullptr);
#endif
}

void log::error(const std::string &message) {
    std::cout << "[" << term::color::red << "ERROR" << term::reset << "] " << message << std::endl;

#ifdef SWITCH
    consoleUpdate(nullptr);
#endif
}