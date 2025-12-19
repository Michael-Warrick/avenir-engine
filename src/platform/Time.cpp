#include "avenir/platform/Time.hpp"

namespace avenir::platform {

Time::Time() { m_lastTime = std::chrono::high_resolution_clock::now(); }

void Time::tick() {
    const auto currentTime = std::chrono::high_resolution_clock::now();
    m_deltaTime =
        std::chrono::duration<float>(currentTime - m_lastTime).count();
    m_lastTime = currentTime;
}

std::chrono::time_point<std::chrono::steady_clock> Time::now() {
    return std::chrono::high_resolution_clock::now();
}

float Time::deltaTime() const { return m_deltaTime; }

}  // namespace avenir::platform