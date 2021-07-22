#ifndef SAVE_SWITCH_LOG_H
#define SAVE_SWITCH_LOG_H

#include <string>

class log {
public:
    static void info(const std::string &message);
    static void warn(const std::string &message);
    static void error(const std::string &message);

};

#endif
