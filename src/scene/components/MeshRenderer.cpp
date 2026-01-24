#include "avenir/scene/components/MeshRenderer.hpp"

namespace avenir::scene::components {

std::unique_ptr<Component> MeshRenderer::clone() const {
    return std::make_unique<MeshRenderer>(*this);
}

std::string MeshRenderer::name() const { return std::string(staticName); }

}  // namespace avenir::scene::components