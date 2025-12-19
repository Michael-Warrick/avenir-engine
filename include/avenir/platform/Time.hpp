#ifndef AVENIR_PLATFORM_TIME_HPP
#define AVENIR_PLATFORM_TIME_HPP

#include <chrono>

namespace avenir::platform {

class Time {
public:
    Time();
    ~Time() = default;

    void tick();

    static std::chrono::time_point<std::chrono::steady_clock> now();
    [[nodiscard]] float deltaTime() const;

private:
    float m_deltaTime = 0.0f;
    std::chrono::time_point<std::chrono::steady_clock> m_lastTime;
};

}  // namespace avenir::platform

#endif  // AVENIR_PLATFORM_TIME_HPP
