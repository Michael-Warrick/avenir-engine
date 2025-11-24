#ifndef AVENIR_SCENE_CAMERA_HPP
#define AVENIR_SCENE_CAMERA_HPP

#include "glm/glm.hpp"

namespace avenir::scene::components {
class Camera {
public:
    enum class MovementDirection { eForward, eBackward, eLeft, eRight };

    Camera(glm::vec3 position, glm::vec3 up, float yaw, float pitch);
    [[nodiscard]] glm::mat4 viewMatrix() const;
    void processKeyboardInput(MovementDirection direction, float deltaTime);
    void processMouseInput(float offsetX, float offsetY,
                           bool useConstrainedPitch);

private:
    void updateCameraVectors();

    glm::vec3 m_position = glm::vec3(0.0f, 0.0f, 0.0f);
    glm::vec3 m_front = glm::vec3(0.0f, 0.0f, 1.0f);
    glm::vec3 m_up = glm::vec3(0.0f, 1.0f, 0.0f);
    glm::vec3 m_right{};
    glm::vec3 m_worldUp{};
    float m_yaw = -90.0f;
    float m_pitch = 0.0f;
    float m_fov = 45.0f;
    float m_movementSpeed = 2.5f;
    float m_mouseSensitivity = 0.1f;
};
}  // namespace avenir::scene::components

#endif  // AVENIR_SCENE_CAMERA_HPP
