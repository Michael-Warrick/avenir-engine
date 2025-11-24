#include "avenir/platform/Window.hpp"

namespace avenir::platform {

Window::Window(const uint32_t width, const uint32_t height, const std::string &title) {
    glfwInit();

    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    glfwWindowHint(GLFW_RESIZABLE, GLFW_TRUE);

    m_glfwWindow =
        glfwCreateWindow(static_cast<int>(width), static_cast<int>(height),
                         title.c_str(), nullptr, nullptr);

    glfwSetWindowUserPointer(m_glfwWindow, &m_context);
}

Window::~Window() {
    glfwDestroyWindow(m_glfwWindow);
    glfwTerminate();
}

bool Window::isOpen() const { return !glfwWindowShouldClose(m_glfwWindow); }

void Window::pollEvents() { glfwPollEvents(); }

uint32_t Window::getWidth() const {
    int w, h;
    glfwGetWindowSize(m_glfwWindow, &w, &h);

    return static_cast<uint32_t>(w);
}

uint32_t Window::getHeight() const {
    int w, h;
    glfwGetWindowSize(m_glfwWindow, &w, &h);

    return static_cast<uint32_t>(h);
}

GLFWwindow *Window::handle() const { return m_glfwWindow; }

Window::Context &Window::context() { return m_context; }

} // avenir::platform