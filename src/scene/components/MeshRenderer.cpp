#include "avenir/scene/components/MeshRenderer.hpp"

namespace avenir::scene::components {

std::string MeshRenderer::name() const { return std::string(staticName); }

}  // namespace avenir::scene::components