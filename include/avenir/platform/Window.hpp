#ifndef WINDOW_HPP
#define WINDOW_HPP

#include <GLFW/glfw3.h>

#include <string>

namespace avenir {
class Window {
public:
    Window() = default;
    Window(uint32_t width, uint32_t height, const std::string &title);
    ~Window();

    [[nodiscard]] bool isOpen() const;
    static void pollEvents();

    [[nodiscard]] GLFWwindow *handle() const;

private:
    GLFWwindow *m_glfwWindow = nullptr;
};
}  // namespace avenir

#endif // WINDOW_HPP