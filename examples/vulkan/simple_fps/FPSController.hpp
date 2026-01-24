#ifndef SIMPLEFPS_FPSCONTROLLER_HPP
#define SIMPLEFPS_FPSCONTROLLER_HPP

#include <avenir/avenir.hpp>

class FPSController {
public:
    FPSController(avenir::Entity &player, avenir::Scene &scene,
                  avenir::InputManager &inputManager);
    ~FPSController() = default;

    void update(float deltaTime);

private:
    void handleKeyboardInput(float deltaTime) const;
    void handleMousePosition();

    avenir::Entity &m_player;
    avenir::Entity *m_camera;

    avenir::Scene &m_scene;

    avenir::InputManager &m_inputManager;

    float m_movementSpeed = 2.5f;
    float m_mouseSensitivity = 0.1f;
    float m_yaw = 0.0f;
    float m_pitch = 0.0f;
    bool m_shouldConstrainLookPitch = true;
};

#endif  // SIMPLEFPS_FPSCONTROLLER_HPP
