#ifndef SIMPLEFPS_FPSCONTROLLER_HPP
#define SIMPLEFPS_FPSCONTROLLER_HPP

#include <avenir/avenir.hpp>

class FPSController {
public:
    FPSController(avenir::InputManager &inputManager, avenir::Entity &camera);
    ~FPSController() = default;

    void update(float deltaTime);

private:
    void handleKeyboardInput(float deltaTime) const;
    void handleMousePosition();

    avenir::InputManager &m_inputManager;
    avenir::Entity &m_camera;

    float m_movementSpeed = 2.5f;
    float m_mouseSensitivity = 0.1f;
    float m_yaw = 0.0f;
    float m_pitch = 0.0f;
    bool m_shouldConstrainLookPitch = true;
};

#endif  // SIMPLEFPS_FPSCONTROLLER_HPP
