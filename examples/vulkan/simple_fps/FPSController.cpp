#include "FPSController.hpp"

#include <avenir/avenir.hpp>

FPSController::FPSController(avenir::InputManager &inputManager,
                             avenir::Entity &camera)
    : m_inputManager(inputManager), m_camera(camera) {
    m_inputManager.setCursorMode(avenir::CursorMode::eDisabled);
}

void FPSController::update(const float deltaTime) {
    handleKeyboardInput(deltaTime);
    handleMousePosition();
}

void FPSController::handleKeyboardInput(const float deltaTime) const {
    auto &playerTransform = m_camera.component<avenir::Transform>();
    const float velocity = m_movementSpeed * deltaTime;

    if (m_inputManager.key(avenir::Key::Code::eW) ==
        avenir::Key::State::ePress) {
        playerTransform.position += playerTransform.forward() * velocity;
    }

    if (m_inputManager.key(avenir::Key::Code::eS) ==
        avenir::Key::State::ePress) {
        playerTransform.position -= playerTransform.forward() * velocity;
    }

    if (m_inputManager.key(avenir::Key::Code::eA) ==
        avenir::Key::State::ePress) {
        playerTransform.position -= playerTransform.right() * velocity;
    }

    if (m_inputManager.key(avenir::Key::Code::eD) ==
        avenir::Key::State::ePress) {
        playerTransform.position += playerTransform.right() * velocity;
    }
}

void FPSController::handleMousePosition() {
    const glm::vec2 mouseOffsets = m_inputManager.mouseDeltas();

    const float offsetX = mouseOffsets.x * m_mouseSensitivity;
    const float offsetY = mouseOffsets.y * m_mouseSensitivity;

    m_yaw += offsetX;
    m_pitch += offsetY;

    if (m_shouldConstrainLookPitch) {
        m_pitch = glm::clamp(m_pitch, -90.0f, 90.0f);
    }

    auto &cameraTransform = m_camera.component<avenir::Transform>();

    // Convert yaw/pitch to quaternions, yaw around world Y (negative is up) and
    // pitch around local X

    const glm::quat yawQuaternion =
        glm::angleAxis(glm::radians(m_yaw), glm::vec3(0, -1, 0));
    const glm::quat pitchQuaternion =
        glm::angleAxis(glm::radians(m_pitch), glm::vec3(1, 0, 0));

    cameraTransform.rotation = glm::normalize(yawQuaternion * pitchQuaternion);
}
