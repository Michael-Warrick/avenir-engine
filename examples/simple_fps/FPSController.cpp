// Highly inspired by Brackey's FIRST PERSON MOVEMENT in Unity tutorial code.

#include "FPSController.hpp"

#include <glm/gtc/quaternion.hpp>
#include <avenir/debug/Debug.hpp>

FPSController::FPSController(avenir::Entity &player, avenir::Scene &scene,
                             avenir::InputManager &inputManager)
    : m_player(player), m_scene(scene), m_inputManager(inputManager) {
    const std::optional<avenir::Entity *> playerCamera =
        findChildEntityWithCameraComponent();
    if (!playerCamera.has_value()) {
        avenir::Debug::log(
            "Failed to locate child entity with camera component!",
            avenir::Debug::MessageSeverity::eError);
        return;
    }

    if (!isCameraMarkedPrimary(
            playerCamera.value()->component<avenir::Camera>())) {
        avenir::Debug::log("Found camera but is not marked as primary!",
                           avenir::Debug::MessageSeverity::eWarning);
        return;
    }

    m_camera = playerCamera.value();
    m_inputManager.setCursorMode(avenir::CursorMode::eDisabled);
}

void FPSController::update(const float deltaTime) {
    handleKeyboardInput(deltaTime);
    handleMousePosition();
}

std::optional<avenir::Entity *>
FPSController::findChildEntityWithCameraComponent() const {
    for (const uint32_t child : m_player.children()) {
        const avenir::Entity &childEntity =
            *m_scene.findEntityById(child).value();
        if (childEntity.hasComponent<avenir::Camera>()) {
            return m_scene.findEntityById(childEntity.id());
        }
    }

    return nullptr;
}

bool FPSController::isCameraMarkedPrimary(const avenir::Camera &camera) {
    if (!camera.isPrimary) {
        return false;
    }

    return true;
}

void FPSController::handleKeyboardInput(const float deltaTime) const {
    auto &playerTransform = m_player.component<avenir::Transform>();

    const float velocity = m_movementSpeed * deltaTime;

    glm::vec3 forward = playerTransform.forward();
    forward.y = 0.0f;
    if (glm::length(forward) > 0.0f) {
        forward = glm::normalize(forward);
    }

    glm::vec3 right = playerTransform.right();
    right.y = 0.0f;
    if (glm::length(right) > 0.0f) {
        right = glm::normalize(right);
    }

    if (m_inputManager.key(avenir::Key::Code::eW) ==
        avenir::Key::State::ePress) {
        playerTransform.position += forward * velocity;
    }

    if (m_inputManager.key(avenir::Key::Code::eS) ==
        avenir::Key::State::ePress) {
        playerTransform.position -= forward * velocity;
    }

    if (m_inputManager.key(avenir::Key::Code::eA) ==
        avenir::Key::State::ePress) {
        playerTransform.position -= right * velocity;
    }

    if (m_inputManager.key(avenir::Key::Code::eD) ==
        avenir::Key::State::ePress) {
        playerTransform.position += right * velocity;
    }
}

void FPSController::handleMousePosition() {
    auto &cameraTransform = m_camera->component<avenir::Transform>();
    auto &playerTransform = m_player.component<avenir::Transform>();

    const glm::vec2 mouseOffsets = m_inputManager.mouseDeltas();

    const float offsetX = mouseOffsets.x * m_mouseSensitivity;
    const float offsetY = mouseOffsets.y * m_mouseSensitivity;

    m_yaw += offsetX;
    m_pitch += offsetY;

    if (m_shouldConstrainLookPitch) {
        m_pitch = glm::clamp(m_pitch, -90.0f, 90.0f);
    }

    // Convert yaw/pitch to quaternions, yaw around world Y (negative is up) and
    // pitch around local X
    const glm::quat yawQuaternion =
        glm::angleAxis(glm::radians(m_yaw), glm::vec3(0, -1, 0));
    const glm::quat pitchQuaternion =
        glm::angleAxis(glm::radians(m_pitch), glm::vec3(1, 0, 0));

    playerTransform.rotation = glm::normalize(yawQuaternion);
    cameraTransform.rotation = glm::normalize(pitchQuaternion);
}
