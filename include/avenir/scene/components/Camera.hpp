#ifndef AVENIR_SCENE_COMPONENTS_CAMERA_HPP
#define AVENIR_SCENE_COMPONENTS_CAMERA_HPP

#include "avenir/scene/Component.hpp"

namespace avenir::scene::components {

struct Camera final : public Component {
    [[nodiscard]] std::string name() const override;

    float fov = 45.0f;
    float nearPlane = 0.1f;
    float farPlane = 100.0f;
    bool isPrimary = true;

    static constexpr std::string_view staticName = "Camera";
};

}  // namespace avenir::scene::components

#endif  // AVENIR_SCENE_COMPONENTS_CAMERA_HPP
