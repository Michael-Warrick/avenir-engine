// Highly inspired by Brackey's FIRST PERSON MOVEMENT in Unity tutorial code.

#include "FPSController.hpp"

#include <glm/gtc/quaternion.hpp>
#include <avenir/debug/debug.hpp>

FPSController::FPSController(avenir::Entity &player, avenir::Scene &scene,
                             avenir::InputManager &inputManager)
    : m_player(player), m_scene(scene), m_inputManager(inputManager) {
    const std::optional<avenir::Entity *> camera =
        findChildEntityWithCameraComponent();
    if (!camera.has_value()) {
        avenir::debug::log(
            "Failed to locate child entity with camera component!",
            avenir::debug::MessageSeverity::eError);
        return;
    }

    if (!isCameraMarkedPrimary(camera.value()->component<avenir::Camera>())) {
        avenir::debug::log("Found camera but is not marked as primary!",
                           avenir::debug::MessageSeverity::eWarning);
        return;
    }

    m_camera = camera.value();
    m_inputManager.setCursorMode(avenir::CursorMode::eDisabled);
}

void FPSController::update(const float deltaTime) {
    handleKeyboardInput(deltaTime);
    handleMousePosition();

    m_velocity.y -= m_gravity * deltaTime;
    m_player.component<avenir::Transform>().position.y +=
        m_velocity.y * deltaTime;

    if (m_player.component<avenir::Transform>().position.y < 0) {
        m_velocity.y = 0;
        m_player.component<avenir::Transform>().position.y = 0;
    }
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

void FPSController::handleKeyboardInput(const float deltaTime) {
    auto &playerTransform = m_player.component<avenir::Transform>();
    const glm::vec3 forward = playerTransform.forward();
    const glm::vec3 right = playerTransform.right();

    m_velocity.x = m_movementSpeed * deltaTime;
    m_velocity.z = m_movementSpeed * deltaTime;

    if (m_inputManager.key(avenir::Key::Code::eW) ==
        avenir::Key::State::ePress) {
        playerTransform.position += forward * m_velocity.z;
    }

    if (m_inputManager.key(avenir::Key::Code::eS) ==
        avenir::Key::State::ePress) {
        playerTransform.position -= forward * m_velocity.z;
    }

    if (m_inputManager.key(avenir::Key::Code::eA) ==
        avenir::Key::State::ePress) {
        playerTransform.position -= right * m_velocity.x;
    }

    if (m_inputManager.key(avenir::Key::Code::eD) ==
        avenir::Key::State::ePress) {
        playerTransform.position += right * m_velocity.x;
    }

    if (m_inputManager.key(avenir::Key::Code::eSpace) ==
            avenir::Key::State::ePress &&
        playerTransform.position.y == 0.0f) {
        m_velocity.y += m_jumpPower;
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