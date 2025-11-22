#include "avenir/graphics/Renderer.hpp"

#include "avenir/graphics/vulkan/VulkanRenderer.hpp"

namespace avenir::graphics {

std::unique_ptr<Renderer> Renderer::create(GLFWwindow *window,
                                           const GraphicsAPI api) {
    switch (api) {
        case GraphicsAPI::eVulkan: {
            auto renderer = std::make_unique<vulkan::VulkanRenderer>(window);
            // Attach renderer instance to GLFW window
            glfwSetWindowUserPointer(window, renderer.get());
            glfwSetFramebufferSizeCallback(window, Renderer::framebufferResizeCallback);

            return renderer;
        }

            // Add more cases as backends are added
        default:
            return nullptr;
            break;
    }
}

void Renderer::framebufferResizeCallback(GLFWwindow *window, const int width,
                                         const int height) {
    if (const auto renderer =
            static_cast<Renderer *>(glfwGetWindowUserPointer(window))) {
        renderer->onFramebufferResize(width, height);
    }
}

}  // namespace avenir::graphics
