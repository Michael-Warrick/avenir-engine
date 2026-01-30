#ifndef AVENIR_GRAPHICS_VULKAN_VULKANDEVICE_HPP
#define AVENIR_GRAPHICS_VULKAN_VULKANDEVICE_HPP

#include <vulkan/vulkan_raii.hpp>

namespace avenir::graphics::vulkan {

class VulkanDevice {
public:
    VulkanDevice(const vk::raii::SurfaceKHR &surface,
                 const vk::raii::PhysicalDevice &physicalDevice,
                 const std::vector<const char *> &physicalDeviceExtensions);
    ~VulkanDevice() = default;

    [[nodiscard]] const vk::raii::Device &handle() const;
    [[nodiscard]] const vk::raii::Queue &queue() const;
    [[nodiscard]] uint32_t queueIndex() const;

private:
    void createDevice();

    const vk::raii::SurfaceKHR &m_surface;

    const vk::raii::PhysicalDevice &m_physicalDevice;
    const std::vector<const char *> &m_physicalDeviceExtensions;

    vk::raii::Device m_device = nullptr;
    vk::raii::Queue m_queue = nullptr;
    uint32_t m_queueIndex = ~0;
};

}  // namespace avenir::graphics::vulkan

#endif  // AVENIR_GRAPHICS_VULKAN_VULKANDEVICE_HPP