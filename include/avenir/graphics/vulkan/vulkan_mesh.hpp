#ifndef AVENIR_GRAPHICS_VULKAN_VULKANMESH_HPP
#define AVENIR_GRAPHICS_VULKAN_VULKANMESH_HPP

#include <vulkan/vulkan_raii.hpp>

#include "avenir/graphics/mesh.hpp"

namespace avenir::graphics::vulkan {

struct VulkanVertex final : public Vertex {
    static vk::VertexInputBindingDescription bindingDescription() {
        return {0, sizeof(VulkanVertex), vk::VertexInputRate::eVertex};
    }

    static std::array<vk::VertexInputAttributeDescription, 3>
    attributeDescriptions() {
        return {vk::VertexInputAttributeDescription(
                    0, 0, vk::Format::eR32G32B32Sfloat,
                    offsetof(VulkanVertex, position)),

                vk::VertexInputAttributeDescription(
                    1, 0, vk::Format::eR32G32B32Sfloat,
                    offsetof(VulkanVertex, color)),

                vk::VertexInputAttributeDescription(
                    2, 0, vk::Format::eR32G32B32Sfloat,
                    offsetof(VulkanVertex, textureCoordinates))};
    }
};

class VulkanMesh final : public Mesh {
public:
    VulkanMesh() = default;
    ~VulkanMesh() override = default;
};

}  // namespace avenir::graphics::vulkan

#endif  // AVENIR_GRAPHICS_VULKAN_VULKANMESH_HPP