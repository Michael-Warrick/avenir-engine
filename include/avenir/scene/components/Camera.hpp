#ifndef AVENIR_SCENE_COMPONENTS_CAMERA_HPP
#define AVENIR_SCENE_COMPONENTS_CAMERA_HPP

#include "glm/glm.hpp"

namespace avenir::scene::components {
class Camera {
public:
    Camera(glm::vec3 position, glm::vec3 up, float yaw, float pitch);

    [[nodiscard]] glm::vec3 position() const;
    [[nodiscard]] glm::vec3 front() const;
    [[nodiscard]] glm::vec3 up() const;
    [[nodiscard]] glm::vec3 right() const;
    [[nodiscard]] glm::vec3 worldUp() const;
    [[nodiscard]] float yaw() const;
    [[nodiscard]] float pitch() const;
    [[nodiscard]] float fov() const;
    [[nodiscard]] glm::mat4 viewMatrix() const;

    void setPosition(glm::vec3 position);
    void setFront(glm::vec3 front);
    void setUp(glm::vec3 up);
    void setRight(glm::vec3 right);
    void setWorldUp(glm::vec3 worldUp);
    void setYaw(float yaw);
    void setPitch(float pitch);
    void setFov(float fov);

private:
    glm::vec3 m_position = glm::vec3(0.0f, 0.0f, 0.0f);
    glm::vec3 m_front = glm::vec3(0.0f, 0.0f, 1.0f);
    glm::vec3 m_up = glm::vec3(0.0f, 1.0f, 0.0f);
    glm::vec3 m_right{};
    glm::vec3 m_worldUp{};
    float m_yaw = -90.0f;
    float m_pitch = 0.0f;
    float m_fov = 45.0f;
};
}  // namespace avenir::scene::components

#endif  // AVENIR_SCENE_COMPONENTS_CAMERA_HPP
