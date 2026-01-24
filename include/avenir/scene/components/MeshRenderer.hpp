#ifndef AVENIR_SCENE_COMPONENTS_MESHRENDERER_HPP
#define AVENIR_SCENE_COMPONENTS_MESHRENDERER_HPP

#include <string>

#include "avenir/scene/Component.hpp"

namespace avenir::scene::components {

struct MeshRenderer final : public Component {
    [[nodiscard]] std::unique_ptr<Component> clone() const override;

    [[nodiscard]] std::string name() const override;
    static constexpr std::string_view staticName = "MeshRenderer";
};

}  // namespace avenir::scene::components

#endif  // AVENIR_SCENE_COMPONENTS_MESHRENDERER_HPP