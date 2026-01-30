#ifndef AVENIR_GRAPHICS_VULKAN_VULKANDEVICE_HPP
#define AVENIR_GRAPHICS_VULKAN_VULKANDEVICE_HPP

#include <vector>

#include <vulkan/vulkan_raii.hpp>

namespace vk {
inline constexpr auto KHRPortabilitySubsetExtensionName =
    "VK_KHR_portability_subset";
}  // namespace vk

namespace avenir::graphics::vulkan {

class VulkanPhysicalDevice {
public:
    VulkanPhysicalDevice(const vk::raii::Instance &instance,
                         const vk::raii::SurfaceKHR &surface);
    ~VulkanPhysicalDevice() = default;

    [[nodiscard]] const vk::raii::PhysicalDevice &handle() const;
    [[nodiscard]] const std::vector<const char *> &extensions() const;

private:
    void pickPhysicalDevice();

    const vk::raii::Instance &m_instance;
    const vk::raii::SurfaceKHR &m_surface;

    vk::raii::PhysicalDevice m_physicalDevice = nullptr;

    const std::vector<const char *> m_physicalDeviceExtensions = {
        vk::KHRSwapchainExtensionName, vk::KHRSpirv14ExtensionName,
        vk::KHRSynchronization2ExtensionName,
        vk::KHRCreateRenderpass2ExtensionName,
#if defined(__APPLE__)
        vk::KHRPortabilitySubsetExtensionName
#endif
    };
};

}  // namespace avenir::graphics::vulkan

#endif  // AVENIR_GRAPHICS_VULKAN_VULKANDEVICE_HPP