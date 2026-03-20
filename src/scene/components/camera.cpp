#include "avenir/scene/components/camera.hpp"

#include "glm/gtc/matrix_transform.hpp"

namespace avenir::scene::components {

std::unique_ptr<Component> Camera::clone() const {
    return std::make_unique<Camera>(*this);
}

std::string Camera::name() const { return std::string(staticName); }

}  // namespace avenir::scene::components
