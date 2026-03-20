#ifndef AVENIR_DEBUG_HPP
#define AVENIR_DEBUG_HPP

#include <string>

namespace avenir::debug {
enum class MessageSeverity { eInformation, eWarning, eError };

void log(const std::string &message, MessageSeverity severity);
}  // namespace avenir::debug
#endif  // AVENIR_DEBUG_HPP