#include "avenir/debug/Debug.hpp"

#include <iostream>

namespace avenir {

void Debug::log(const std::string &message, const MessageSeverity severity) {
#ifndef NDEBUG
    if (severity == MessageSeverity::eInformation) {
        std::cout << "\e[1;34m[INFO]\e[0m \e[0;34m" << message << "\e[0m\n";
    } else if (severity == MessageSeverity::eWarning) {
        std::cout << "\e[1;33m[WARNING]\e[0m \e[0;33m" << message << "\e[0m\n";
    } else if (severity == MessageSeverity::eError) {
        std::cout << "\e[1;31m[ERROR]\e[0m \e[0;31m" << message << "\e[0m\n";
    }
}
#endif  // NDEBUG

// Do nothing in release builds?
}  // namespace avenir