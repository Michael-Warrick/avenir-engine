#ifndef SIMPLEFPS_FPSCONTROLLER_HPP
#define SIMPLEFPS_FPSCONTROLLER_HPP

#include "avenir/input/InputManager.hpp"
#include "avenir/scene/components/Camera.hpp"

class FPSController {
public:
    FPSController(avenir::input::InputManager &inputManager,
                  avenir::scene::components::Camera &camera);
    ~FPSController() = default;

    void update(float deltaTime) const;

private:
    void handleKeyboardInput(float deltaTime) const;
    void handleMousePosition() const;
    void updateCameraVectors() const;

    avenir::input::InputManager &m_inputManager;
    avenir::scene::components::Camera &m_camera;

    float m_movementSpeed = 2.5f;
    float m_mouseSensitivity = 0.1f;
    bool m_shouldConstrainLookPitch = true;
};

#endif  // AVENIR_FPSCONTROLLER_HPP
