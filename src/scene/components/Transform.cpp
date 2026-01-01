//
// Created by Michael Warrick on 28/12/2025.
//

#include "avenir/scene/components/Transform.hpp"

#include <glm/gtc/quaternion.hpp>
#include <glm/gtc/matrix_transform.hpp>

namespace avenir::scene::components {

Transform::Transform(const glm::vec3 position) { this->position = position; }

Transform::Transform(const glm::vec3 position, const glm::quat rotation,
                     const glm::vec3 scale) {
    this->position = position;
    this->rotation = rotation;
    this->scale = scale;
}

std::string Transform::name() const { return "Transform"; }

glm::vec3 Transform::forward() const {
    return glm::normalize(rotation * glm::vec3(0.0f, 0.0f, -1.0f));
}

glm::vec3 Transform::up() const {
    return glm::normalize(rotation * glm::vec3(0.0f, 1.0f, 0.0f));
}

glm::vec3 Transform::right() const {
    return glm::normalize(rotation * glm::vec3(1.0f, 0.0f, 0.0f));
}

glm::mat4 Transform::worldMatrix() const {
    const glm::mat4 T = glm::translate(glm::mat4(1.0f), position);
    const glm::mat4 R = glm::mat4_cast(rotation);
    const glm::mat4 S = glm::scale(glm::mat4(1.0f), scale);

    return T * R * S;
}

glm::mat4 Transform::inverseWorldMatrix() const {
    return glm::inverse(worldMatrix());
}

}  // namespace avenir::scene::components