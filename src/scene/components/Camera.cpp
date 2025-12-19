#include "avenir/scene/components/Camera.hpp"

#include "glm/gtc/matrix_transform.hpp"

namespace avenir::scene::components {

Camera::Camera(const glm::vec3 position, glm::vec3 up, float yaw, float pitch)
    : m_position(position), m_up(up), m_yaw(yaw), m_pitch(pitch) {
    m_worldUp = glm::normalize(up);

    glm::vec3 front;
    front.x = std::cos(glm::radians(m_yaw)) * std::cos(glm::radians(m_pitch));
    front.y = std::sin(glm::radians(m_pitch));
    front.z = std::sin(glm::radians(m_yaw)) * std::cos(glm::radians(m_pitch));

    m_front = glm::normalize(front);
    m_right = glm::normalize(glm::cross(m_front, m_worldUp));
    m_up = glm::normalize(glm::cross(m_right, m_front));
}

glm::vec3 Camera::position() const { return m_position; }

glm::vec3 Camera::front() const { return m_front; }

glm::vec3 Camera::up() const { return m_up; }

glm::vec3 Camera::right() const { return m_right; }

glm::vec3 Camera::worldUp() const { return m_worldUp; }

float Camera::yaw() const { return m_yaw; }

float Camera::pitch() const { return m_pitch; }

float Camera::fov() const { return m_fov; }

glm::mat4 Camera::viewMatrix() const {
    return glm::lookAt(m_position, m_position + m_front, m_up);
}

void Camera::setPosition(const glm::vec3 position) { m_position = position; }

void Camera::setFront(const glm::vec3 front) { m_front = front; }

void Camera::setUp(const glm::vec3 up) { m_up = up; }

void Camera::setRight(const glm::vec3 right) { m_right = right; }

void Camera::setWorldUp(const glm::vec3 worldUp) { m_worldUp = worldUp; }

void Camera::setYaw(const float yaw) { m_yaw = yaw; }

void Camera::setPitch(const float pitch) { m_pitch = pitch; }

void Camera::setFov(const float fov) { m_fov = fov; }

}  // namespace avenir::scene::components
