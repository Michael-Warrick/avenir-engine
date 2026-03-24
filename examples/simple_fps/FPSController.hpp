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
    [[nodiscard]] std::optional<avenir::Entity *>
    findChildEntityWithCameraComponent() const;

    static bool isCameraMarkedPrimary(const avenir::Camera &camera);

    void handleKeyboardInput(float deltaTime);
    void handleMousePosition();

    void jump();

    // Camera entity cannot be a reference as it would need to be initialized in
    // the initializer list of FPSController's ctor.
    avenir::Entity *m_camera;

    avenir::Entity &m_player;
    avenir::Scene &m_scene;
    avenir::InputManager &m_inputManager;

    float m_yaw = 0.0f;
    float m_pitch = 0.0f;
    bool m_shouldConstrainLookPitch = true;
    float m_mouseSensitivity = 0.1f;

    float m_movementSpeed = 2.5f;
    glm::vec3 m_velocity = glm::vec3(0.0f);
    float m_jumpPower = 3.0f;
    float m_cameraHeight = 0.8f;
    static constexpr float m_gravity = 9.80665f;  // m/s^2
};

#endif  // SIMPLEFPS_FPSCONTROLLER_HPP
