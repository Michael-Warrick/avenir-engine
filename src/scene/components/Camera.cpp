#include "avenir/scene/components/Camera.hpp"

#include "glm/gtc/matrix_transform.hpp"

namespace avenir::scene::components {

Camera::Camera(glm::vec3 position, glm::vec3 up, float yaw, float pitch)
    : m_position(position), m_up(up), m_yaw(yaw), m_pitch(pitch) {
    m_worldUp = glm::normalize(up);
    updateCameraVectors();
}

glm::mat4 Camera::viewMatrix() const {
    return glm::lookAt(m_position, m_position + m_front, m_up);
}

void Camera::processKeyboardInput(const MovementDirection direction,
                                  const float deltaTime) {
    const float velocity = m_movementSpeed * deltaTime;
    if (direction == MovementDirection::eForward) {
        m_position += m_front * velocity;
    }
    if (direction == MovementDirection::eBackward) {
        m_position -= m_front * velocity;
    }
    if (direction == MovementDirection::eLeft) {
        m_position -= m_right * velocity;
    }
    if (direction == MovementDirection::eRight) {
        m_position += m_right * velocity;
    }
}

void Camera::processMouseInput(float offsetX, float offsetY,
                               const bool useConstrainedPitch) {
    offsetX *= m_mouseSensitivity;
    offsetY *= m_mouseSensitivity;

    m_yaw += offsetX;
    m_pitch += offsetY;

    if (useConstrainedPitch) {
        if (m_pitch >= 90.0f) {
            m_pitch = 90.0f;
        }
        if (m_pitch <= -90.0f) {
            m_pitch = -90.0f;
        }
    }

    updateCameraVectors();
}


void Camera::updateCameraVectors() {
    glm::vec3 front;
    front.x = std::cos(glm::radians(m_yaw)) * std::cos(glm::radians(m_pitch));
    front.y = std::sin(glm::radians(m_pitch));
    front.z = std::sin(glm::radians(m_yaw)) * std::cos(glm::radians(m_pitch));
    m_front = glm::normalize(front);

    m_right = glm::normalize(glm::cross(m_front, m_worldUp));
    m_up = glm::normalize(glm::cross(m_right, m_front));
}

}  // namespace avenir::scene::components
