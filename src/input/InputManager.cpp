#include "avenir/input/InputManager.hpp"

namespace avenir::input {

InputManager::InputManager(avenir::platform::Window &window,
                           avenir::scene::components::Camera &camera)
    : m_window(window), m_camera(camera) {
    GLFWwindow *glfwWindow = window.handle();
    window.context().inputManager = this;
    glfwSetCursorPosCallback(glfwWindow, &InputManager::mouseCursorPositionCallback);

    // Seed initial mouse position to center of window
    m_lastMousePositionX = static_cast<float>(window.getWidth()) / 2.0f;
    m_lastMousePositionY = static_cast<float>(window.getHeight()) / 2.0f;
}

void InputManager::update(float deltaTime) {
    handleKeyboardInput(deltaTime);
}

void InputManager::handleKeyboardInput(float deltaTime) {
    using namespace avenir::scene::components;

    GLFWwindow *glfwWindow = m_window.handle();
    if (glfwGetKey(glfwWindow, GLFW_KEY_W) == GLFW_PRESS) {
        m_camera.processKeyboardInput(Camera::MovementDirection::eForward,
                                     deltaTime);
    }
    if (glfwGetKey(glfwWindow, GLFW_KEY_S) == GLFW_PRESS) {
        m_camera.processKeyboardInput(Camera::MovementDirection::eBackward,
                                     deltaTime);
    }
    if (glfwGetKey(glfwWindow, GLFW_KEY_A) == GLFW_PRESS) {
        m_camera.processKeyboardInput(Camera::MovementDirection::eLeft,
                                     deltaTime);
    }
    if (glfwGetKey(glfwWindow, GLFW_KEY_D) == GLFW_PRESS) {
        m_camera.processKeyboardInput(Camera::MovementDirection::eRight,
                                     deltaTime);
    }
}


void InputManager::handleMouseCursorPositionMove(double mousePositionX,
                                                 double mousePositionY) {
    GLFWwindow *handle = m_window.handle();

    const auto positionX = static_cast<float>(mousePositionX);
    const auto positionY = static_cast<float>(mousePositionY);

    if (m_isFirstMouse) {
        m_lastMousePositionX = positionX;
        m_lastMousePositionY = positionY;
        m_isFirstMouse = false;
    }

    const bool focused = glfwGetWindowAttrib(handle, GLFW_FOCUSED);
    const bool isHoldingRightClickDown = glfwGetMouseButton(handle, GLFW_MOUSE_BUTTON_RIGHT) == GLFW_PRESS;
    const bool shouldLook = focused && isHoldingRightClickDown;

    if (!shouldLook) {
        if (m_isLooking) {
            // Just left look mode
            m_isLooking = false;
            m_isFirstMouse = true;
            m_ignoreNextDelta = false;
            glfwSetInputMode(handle, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
        }

        m_lastMousePositionX = positionX;
        m_lastMousePositionY = positionY;
        return;
    }

    if (!m_isLooking) {
        m_isLooking = true;
        m_isFirstMouse = true;
        m_ignoreNextDelta = true;

        glfwSetInputMode(handle, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

        m_lastMousePositionX = positionX;
        m_lastMousePositionY = positionY;
        return;
    }

    if (m_ignoreNextDelta) {
        m_ignoreNextDelta = false;
        m_lastMousePositionX = positionX;
        m_lastMousePositionY = positionY;
        return;
    }

    const float offsetX = positionX - m_lastMousePositionX;
    const float offsetY = m_lastMousePositionY - positionY;

    m_lastMousePositionX = positionX;
    m_lastMousePositionY = positionY;

    m_camera.processMouseInput(offsetX, offsetY, true);
}

void InputManager::mouseCursorPositionCallback(GLFWwindow *window,
                                               double mousePositionX,
                                               double mousePositionY) {
    auto *windowContext = static_cast<platform::Window::Context*>(glfwGetWindowUserPointer(window));
    if (!windowContext || !windowContext->inputManager) {
        return;
    }

    windowContext->inputManager->handleMouseCursorPositionMove(mousePositionX, mousePositionY);
}

}  // namespace avenir::input