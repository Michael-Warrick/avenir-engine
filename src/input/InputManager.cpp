#include "avenir/input/InputManager.hpp"

#include "GLFW/glfw3.h"

namespace avenir::input {

InputManager::InputManager(avenir::platform::Window &window)
    : m_window(window) {
    GLFWwindow *glfwWindow = window.handle();
    window.context().inputManager = this;
    glfwSetCursorPosCallback(glfwWindow,
                             &InputManager::mouseCursorPositionCallback);

    // Seed initial mouse position to center of window
    m_lastMousePositionX = static_cast<float>(window.width()) / 2.0f;
    m_lastMousePositionY = static_cast<float>(window.height()) / 2.0f;
}

Key::State InputManager::key(Key::Code code) const {
    GLFWwindow *handle = m_window.handle();
    return static_cast<Key::State>(glfwGetKey(handle, static_cast<int>(code)));
}

glm::vec2 InputManager::mouseDeltas() {
    const glm::vec2 deltas{m_mouseOffsetX, m_mouseOffsetY};
    m_mouseOffsetX = 0.0f;
    m_mouseOffsetY = 0.0f;

    return deltas;
}

void InputManager::setCursorMode(const CursorMode mode) const {
    switch (mode) {
        case CursorMode::eNormal:
            glfwSetInputMode(m_window.handle(), GLFW_CURSOR,
                             GLFW_CURSOR_NORMAL);
            break;

        case CursorMode::eHidden:
            glfwSetInputMode(m_window.handle(), GLFW_CURSOR,
                             GLFW_CURSOR_HIDDEN);
            break;

        case CursorMode::eDisabled:
            glfwSetInputMode(m_window.handle(), GLFW_CURSOR,
                             GLFW_CURSOR_DISABLED);
            break;

        case CursorMode::eCaptured:
            glfwSetInputMode(m_window.handle(), GLFW_CURSOR,
                             GLFW_CURSOR_CAPTURED);
            break;

        default:
            break;
    }
}

void InputManager::handleMouseCursorPositionMove(const double mousePositionX,
                                                 const double mousePositionY) {
    GLFWwindow *handle = m_window.handle();

    const auto positionX = static_cast<float>(mousePositionX);
    const auto positionY = static_cast<float>(mousePositionY);

    if (m_isFirstMouse) {
        m_lastMousePositionX = positionX;
        m_lastMousePositionY = positionY;
        m_isFirstMouse = false;
    }

    m_mouseOffsetX = positionX - m_lastMousePositionX;
    m_mouseOffsetY = m_lastMousePositionY - positionY;

    m_lastMousePositionX = positionX;
    m_lastMousePositionY = positionY;
}

void InputManager::mouseCursorPositionCallback(GLFWwindow *window,
                                               const double mousePositionX,
                                               const double mousePositionY) {
    const auto *windowContext = static_cast<platform::Window::Context *>(
        glfwGetWindowUserPointer(window));
    if (!windowContext || !windowContext->inputManager) {
        return;
    }

    windowContext->inputManager->handleMouseCursorPositionMove(mousePositionX,
                                                               mousePositionY);
}

}  // namespace avenir::input