#include "avenir/graphics/renderer.hpp"

#include "avenir/graphics/vulkan/vulkan_renderer.hpp"
#include "avenir/platform/window.hpp"

namespace avenir::graphics {

std::unique_ptr<Renderer> Renderer::create(platform::Window &window,
                                           const Api api) {
    switch (api) {
        case Api::eVulkan: {
            auto renderer = std::make_unique<vulkan::VulkanRenderer>(window.handle());
            // Attach renderer instance to GLFW window
            window.context().renderer = renderer.get();
            glfwSetFramebufferSizeCallback(window.handle(), framebufferResizeCallback);

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
    auto *windowContext = static_cast<platform::Window::Context *>(glfwGetWindowUserPointer(window));
    if (!windowContext || !windowContext->renderer) {
        return;
    }

    windowContext->renderer->onFramebufferResize(width, height);
}

}  // namespace avenir::graphics
