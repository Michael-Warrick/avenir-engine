#ifndef AVENIR_DEBUG_HPP
#define AVENIR_DEBUG_HPP

#include <string>

namespace avenir {
class Debug {
public:
    enum class MessageSeverity { eInformation, eWarning, eError };

    static void log(const std::string &message, MessageSeverity severity);
};
}  // namespace avenir
#endif  // AVENIR_DEBUG_HPP