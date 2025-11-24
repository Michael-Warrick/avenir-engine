#ifndef AVENIR_PLATFORM_INPUTMANAGER_HPP
#define AVENIR_PLATFORM_INPUTMANAGER_HPP

#include "GLFW/glfw3.h"
#include "avenir/platform/Window.hpp"
#include "avenir/scene/components/Camera.hpp"

namespace avenir::input {

class InputManager {
public:
    InputManager(platform::Window &window, avenir::scene::components::Camera &camera);
    ~InputManager() = default;

    void update(float deltaTime);

private:
    void handleKeyboardInput(float deltaTime);
    void handleMouseCursorPositionMove(double mousePositionX, double mousePositionY);
    static void mouseCursorPositionCallback(GLFWwindow *window, double mousePositionX, double mousePositionY);

    avenir::platform::Window &m_window;
    avenir::scene::components::Camera &m_camera;

    bool m_isFirstMouse = true;
    bool m_isLooking = false;
    bool m_ignoreNextDelta = false;
    float m_lastMousePositionX = 0.0f;
    float m_lastMousePositionY = 0.0f;
};

} // namespace avenir::input

#endif // AVENIR_PLATFORM_INPUTMANAGER_HPP