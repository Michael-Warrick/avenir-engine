#ifndef AVENIR_GRAPHICS_VULKAN_VULKANSURFACE_HPP
#define AVENIR_GRAPHICS_VULKAN_VULKANSURFACE_HPP

#include <vulkan/vulkan_raii.hpp>

// REQUIRED only for GLFW CreateWindowSurface.
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

namespace glfw {
struct Window;
}  // namespace glfw

namespace avenir::graphics::vulkan {

class VulkanSurface {
public:
    VulkanSurface(const vk::raii::Instance &instance, GLFWwindow *window);
    ~VulkanSurface() = default;

    [[nodiscard]] const vk::raii::SurfaceKHR &handle() const;

private:
    void createSurface();

    const vk::raii::Instance &m_instance;
    GLFWwindow *m_glfwWindow;

    vk::raii::SurfaceKHR m_surface = nullptr;
};

}  // namespace avenir::graphics::vulkan

#endif  // AVENIR_GRAPHICS_VULKAN_VULKANSURFACE_HPP