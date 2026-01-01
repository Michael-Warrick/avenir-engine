#include "avenir/scene/components/Camera.hpp"

#include "glm/gtc/matrix_transform.hpp"

namespace avenir::scene::components {

std::string Camera::name() const { return std::string(staticName); }

}  // namespace avenir::scene::components
