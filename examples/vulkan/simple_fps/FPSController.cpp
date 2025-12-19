#include "FPSController.hpp"

using namespace avenir::input;
using namespace avenir::scene::components;

FPSController::FPSController(InputManager &inputManager, Camera &camera)
    : m_inputManager(inputManager), m_camera(camera) {
    m_inputManager.setCursorMode(CursorMode::eDisabled);
}

void FPSController::update(const float deltaTime) const {
    handleKeyboardInput(deltaTime);
    handleMousePosition();
}

void FPSController::handleKeyboardInput(const float deltaTime) const {
    const float velocity = m_movementSpeed * deltaTime;

    if (m_inputManager.key(Key::Code::eW) == Key::State::ePress) {
        m_camera.setPosition(m_camera.position() + m_camera.front() * velocity);
    }

    if (m_inputManager.key(Key::Code::eS) == Key::State::ePress) {
        m_camera.setPosition(m_camera.position() - m_camera.front() * velocity);
    }

    if (m_inputManager.key(Key::Code::eA) == Key::State::ePress) {
        m_camera.setPosition(m_camera.position() - m_camera.right() * velocity);
    }

    if (m_inputManager.key(Key::Code::eD) == Key::State::ePress) {
        m_camera.setPosition(m_camera.position() + m_camera.right() * velocity);
    }
}

void FPSController::handleMousePosition() const {
    const glm::vec2 mouseOffsets = m_inputManager.mouseDeltas();

    float offsetX = mouseOffsets.x;
    float offsetY = mouseOffsets.y;

    offsetX *= m_mouseSensitivity;
    offsetY *= m_mouseSensitivity;

    m_camera.setYaw(m_camera.yaw() + offsetX);
    m_camera.setPitch(m_camera.pitch() + offsetY);

    if (m_shouldConstrainLookPitch) {
        if (m_camera.pitch() >= 90.0f) {
            m_camera.setPitch(90.0f);
        }
        if (m_camera.pitch() <= -90.0f) {
            m_camera.setPitch(-90.0f);
        }
    }

    updateCameraVectors();
}

void FPSController::updateCameraVectors() const {
    glm::vec3 front;
    front.x = std::cos(glm::radians(m_camera.yaw())) *
              std::cos(glm::radians(m_camera.pitch()));
    front.y = std::sin(glm::radians(m_camera.pitch()));
    front.z = std::sin(glm::radians(m_camera.yaw())) *
              std::cos(glm::radians(m_camera.pitch()));

    m_camera.setFront(glm::normalize(front));
    m_camera.setRight(
        glm::normalize(glm::cross(m_camera.front(), m_camera.worldUp())));
    m_camera.setUp(
        glm::normalize(glm::cross(m_camera.right(), m_camera.front())));
}
