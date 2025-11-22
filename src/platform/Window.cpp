#include "avenir/platform/Window.hpp"

namespace avenir {

Window::Window(const uint32_t width, const uint32_t height, const std::string &title) {
    glfwInit();

    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    glfwWindowHint(GLFW_RESIZABLE, GLFW_TRUE);

    m_glfwWindow =
        glfwCreateWindow(static_cast<int>(width), static_cast<int>(height),
                         title.c_str(), nullptr, nullptr);
}

Window::~Window() {
    glfwDestroyWindow(m_glfwWindow);
    glfwTerminate();
}

bool Window::isOpen() const { return !glfwWindowShouldClose(m_glfwWindow); }

void Window::pollEvents() { glfwPollEvents(); }

GLFWwindow *Window::handle() const { return m_glfwWindow; }

} // avenir