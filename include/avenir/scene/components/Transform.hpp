//
// Created by Michael Warrick on 28/12/2025.
//

#ifndef AVENIR_TRANSFORM_HPP
#define AVENIR_TRANSFORM_HPP

#include <string>

#include <glm/vec3.hpp>
#include <glm/mat4x4.hpp>
#include <glm/gtc/quaternion.hpp>

#include "avenir/scene/Component.hpp"

namespace avenir::scene::components {

struct Transform final : public Component {
    Transform() = default;
    explicit Transform(glm::vec3 position);
    Transform(glm::vec3 position, glm::quat rotation, glm::vec3 scale);
    [[nodiscard]] std::string name() const override;

    glm::vec3 position = glm::vec3(0.0f, 0.0f, 0.0f);
    glm::quat rotation = glm::quat(1.0f, 0.0f, 0.0f, 0.0f);
    glm::vec3 scale = glm::vec3(1.0f, 1.0f, 1.0f);

    [[nodiscard]] glm::vec3 forward() const;
    [[nodiscard]] glm::vec3 up() const;
    [[nodiscard]] glm::vec3 right() const;
    [[nodiscard]] glm::mat4 worldMatrix() const;
    [[nodiscard]] glm::mat4 inverseWorldMatrix() const;
};

}  // namespace avenir::scene::components

#endif  // AVENIR_TRANSFORM_HPP
