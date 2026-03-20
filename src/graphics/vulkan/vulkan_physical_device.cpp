#include "avenir/graphics/vulkan/vulkan_physical_device.hpp"

#include "avenir/debug/debug.hpp"

namespace avenir::graphics::vulkan {

VulkanPhysicalDevice::VulkanPhysicalDevice(const vk::raii::Instance &instance,
                                           const vk::raii::SurfaceKHR &surface)
    : m_instance(instance), m_surface(surface) {
    pickPhysicalDevice();
}

const vk::raii::PhysicalDevice &VulkanPhysicalDevice::handle() const {
    return m_physicalDevice;
}

const std::vector<const char *> &VulkanPhysicalDevice::extensions() const {
    return m_physicalDeviceExtensions;
}

void VulkanPhysicalDevice::pickPhysicalDevice() {
    std::vector<vk::raii::PhysicalDevice> physicalDevices =
        m_instance.enumeratePhysicalDevices();

    const auto physicalDeviceIterator =
        std::ranges::find_if(physicalDevices, [&](auto const &physicalDevice) {
            // Check if the physical device supports Vulkan 1.3 or up...
            const bool supportsVulkan13 =
                physicalDevice.getProperties().apiVersion >= VK_API_VERSION_1_3;

            // Check if any of available queue families have support for
            // graphics operations
            auto queueFamilies = physicalDevice.getQueueFamilyProperties();
            const bool supportsGraphicsOperations = std::ranges::any_of(
                queueFamilies, [](auto const &queueFamilyProperties) {
                    return !!(queueFamilyProperties.queueFlags &
                              vk::QueueFlagBits::eGraphics);
                });

            // Check all required physical device extensions are available
            auto availableExtensions =
                physicalDevice.enumerateDeviceExtensionProperties();
            const bool supportsAllRequiredExtensions = std::ranges::all_of(
                m_physicalDeviceExtensions,
                [&availableExtensions](auto const &requiredExtension) {
                    return std::ranges::any_of(
                        availableExtensions,
                        [requiredExtension](auto const &availableExtension) {
                            return strcmp(availableExtension.extensionName,
                                          requiredExtension) == 0;
                        });
                });

            auto features = physicalDevice.template getFeatures2<
                vk::PhysicalDeviceFeatures2, vk::PhysicalDeviceVulkan11Features,
                vk::PhysicalDeviceVulkan13Features,
                vk::PhysicalDeviceExtendedDynamicStateFeaturesEXT>();

            const bool supportsRequiredFeatures =
                features.template get<vk::PhysicalDeviceFeatures2>()
                    .features.samplerAnisotropy &&
                features.template get<vk::PhysicalDeviceVulkan11Features>()
                    .shaderDrawParameters &&
                features.template get<vk::PhysicalDeviceVulkan13Features>()
                    .synchronization2 &&
                features.template get<vk::PhysicalDeviceVulkan13Features>()
                    .dynamicRendering &&
                features
                    .template get<
                        vk::PhysicalDeviceExtendedDynamicStateFeaturesEXT>()
                    .extendedDynamicState;

            return supportsVulkan13 && supportsGraphicsOperations &&
                   supportsAllRequiredExtensions && supportsRequiredFeatures;
        });

    if (physicalDeviceIterator != physicalDevices.end()) {
        m_physicalDevice = *physicalDeviceIterator;
    } else {
        throw std::runtime_error(
            "[Vulkan] Error: Failed to find a suitable GPU!\n");
    }

    debug::log("[Vulkan] Created: PhysicalDevice",
               debug::MessageSeverity::eInformation);
}

}  // namespace avenir::graphics::vulkan