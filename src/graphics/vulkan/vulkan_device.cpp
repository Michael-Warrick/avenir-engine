#include "avenir/graphics/vulkan/vulkan_device.hpp"

#include "avenir/debug/debug.hpp"

namespace avenir::graphics::vulkan {

VulkanDevice::VulkanDevice(
    const vk::raii::SurfaceKHR &surface,
    const vk::raii::PhysicalDevice &physicalDevice,
    const std::vector<const char *> &physicalDeviceExtensions)
    : m_surface(surface),
      m_physicalDevice(physicalDevice),
      m_physicalDeviceExtensions(physicalDeviceExtensions) {
    createDevice();
}

const vk::raii::Device &VulkanDevice::handle() const { return m_device; }

const vk::raii::Queue &VulkanDevice::queue() const { return m_queue; }

uint32_t VulkanDevice::queueIndex() const { return m_queueIndex; }

void VulkanDevice::createDevice() {
    std::vector<vk::QueueFamilyProperties> queueFamilyProperties =
        m_physicalDevice.getQueueFamilyProperties();

    // Get first index into queueFamilyProperties which supports both graphics
    // and presentation operations
    for (uint32_t queueFamilyPropertyIndex = 0;
         queueFamilyPropertyIndex < queueFamilyProperties.size();
         queueFamilyPropertyIndex++) {
        if ((queueFamilyProperties[queueFamilyPropertyIndex].queueFlags &
             vk::QueueFlagBits::eGraphics) &&
            m_physicalDevice.getSurfaceSupportKHR(queueFamilyPropertyIndex,
                                                  *m_surface)) {
            // Found a queue family that supports both graphics and
            // presentation!
            m_queueIndex = queueFamilyPropertyIndex;
            break;
        }
    }

    if (m_queueIndex == ~0) {
        throw std::runtime_error(
            "[Vulkan] Error: Couldn't find a queue "
            "supporting both graphics and presentation!");
    }

    // Query for Vulkan 1.3+ features
    vk::StructureChain<vk::PhysicalDeviceFeatures2,
                       vk::PhysicalDeviceVulkan11Features,
                       vk::PhysicalDeviceVulkan13Features,
                       vk::PhysicalDeviceExtendedDynamicStateFeaturesEXT>
        featureChain(
            vk::PhysicalDeviceFeatures2{}.features = {{.samplerAnisotropy =
                                                           vk::True}},
            vk::PhysicalDeviceVulkan11Features{}.setShaderDrawParameters(
                vk::True),
            vk::PhysicalDeviceVulkan13Features{}
                .setDynamicRendering(vk::True)
                .setSynchronization2(vk::True),
            vk::PhysicalDeviceExtendedDynamicStateFeaturesEXT{}
                .setExtendedDynamicState(vk::True));

    // Create a logical device
    float queuePriority = 1.0f;
    vk::DeviceQueueCreateInfo deviceQueueCreateInfo =
        vk::DeviceQueueCreateInfo()
            .setQueueFamilyIndex(m_queueIndex)
            .setQueueCount(1)
            .setPQueuePriorities(&queuePriority);

    vk::DeviceCreateInfo logicalDeviceCreateInfo =
        vk::DeviceCreateInfo()
            .setPNext(&featureChain.get<vk::PhysicalDeviceFeatures2>())
            .setQueueCreateInfoCount(1)
            .setPQueueCreateInfos(&deviceQueueCreateInfo)
            .setEnabledExtensionCount(
                static_cast<uint32_t>(m_physicalDeviceExtensions.size()))
            .setPpEnabledExtensionNames(m_physicalDeviceExtensions.data());

    m_device = vk::raii::Device(m_physicalDevice, logicalDeviceCreateInfo);
    m_queue = vk::raii::Queue(m_device, m_queueIndex, 0);

    debug::log("[Vulkan] Created: Device",
               debug::MessageSeverity::eInformation);
    debug::log("[Vulkan] Created: Queue (Graphics and Presentation)",
               debug::MessageSeverity::eInformation);
}

}  // namespace avenir::graphics::vulkan