#ifndef SAVE_SWITCH_TERMINAL_H
#define SAVE_SWITCH_TERMINAL_H

#include <string>
#include <sstream>

namespace term {
    enum color {
        black,
        red,
        green,
        yellow,
        blue,
        magenta,
        cyan,
        white
    };

    std::ostream & reset(std::ostream &os) {
        return os << "\u001b[0m";
    }
};

std::ostream &operator<<(std::ostream &os, const term::color &color) {
    return os << "\u001b[3" + std::to_string(color) + "m";
}

#endif
