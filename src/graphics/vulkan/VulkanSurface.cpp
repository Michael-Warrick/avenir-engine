#include "avenir/graphics/vulkan/VulkanSurface.hpp"

#include "avenir/debug/Debug.hpp"

namespace avenir::graphics::vulkan {

VulkanSurface::VulkanSurface(const vk::raii::Instance &instance,
                             GLFWwindow *window)
    : m_instance(instance), m_glfwWindow(window) {
    createSurface();
}

const vk::raii::SurfaceKHR &VulkanSurface::handle() const { return m_surface; }

void VulkanSurface::createSurface() {
    VkSurfaceKHR surface;
    if (glfwCreateWindowSurface(*m_instance, m_glfwWindow, nullptr, &surface) !=
        VK_SUCCESS) {
        throw std::runtime_error(
            "[Vulkan] Error: Failed to create window surface!");
    }

    m_surface = vk::raii::SurfaceKHR(m_instance, surface);

    Debug::log("[Vulkan] Created: Surface",
               Debug::MessageSeverity::eInformation);
}

}  // namespace avenir::graphics::vulkan