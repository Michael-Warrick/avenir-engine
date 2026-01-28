#include "avenir/graphics/vulkan/VulkanInstance.hpp"

#include <GLFW/glfw3.h>
#include "avenir/debug/Debug.hpp"

#include <iostream>

namespace avenir::graphics::vulkan {

VulkanInstance::VulkanInstance() {
    createInstance();
    setupDebugMessenger();
}

const vk::raii::Instance &VulkanInstance::instance() const {
    return m_instance;
}

void VulkanInstance::printAllAvailableInstanceExtensions() const {
    Debug::log("[Vulkan] Available instance extensions:",
               Debug::MessageSeverity::eInformation);

    auto extensions = m_context.enumerateInstanceExtensionProperties();
    for (const auto &extension : extensions) {
        std::string extensionName =
            "\t\t" + static_cast<std::string>(extension.extensionName.data());
        Debug::log(extensionName, Debug::MessageSeverity::eInformation);
    }
}

std::vector<const char *> VulkanInstance::findRequiredInstanceLayers() const {
    // Retrieve the required layers
    std::vector<char const *> layers;
    if (m_shouldUseValidationLayers) {
        layers.assign(m_validationLayers.begin(), m_validationLayers.end());
    }

    // Check if required layers are supported by current Vulkan implementation
    if (auto instanceLayerProperties =
            m_context.enumerateInstanceLayerProperties();
        std::ranges::any_of(
            layers, [&instanceLayerProperties](auto const &requiredLayer) {
                return std::ranges::none_of(
                    instanceLayerProperties,
                    [requiredLayer](auto const &instanceLayerProperty) {
                        return strcmp(instanceLayerProperty.layerName,
                                      requiredLayer) == 0;
                    });
            })) {
        throw std::runtime_error(
            "[Vulkan] Error: One or more required layers are not supported!\n");
    }

    return layers;
}

std::vector<const char *> VulkanInstance::findRequiredInstanceExtensions() {
    uint32_t glfwExtensionCount = 0;
    const auto glfwExtensions =
        glfwGetRequiredInstanceExtensions(&glfwExtensionCount);

    std::vector<const char *> extensions(glfwExtensions,
                                         glfwExtensions + glfwExtensionCount);

#if defined(__APPLE__)
    extensions.push_back(vk::KHRPortabilityEnumerationExtensionName);
#endif

    if (m_shouldUseValidationLayers) {
        extensions.push_back(vk::EXTDebugUtilsExtensionName);
    }

    return extensions;
}

vk::Bool32 VulkanInstance::debugCallback(
    const vk::DebugUtilsMessageSeverityFlagBitsEXT severity,
    const vk::DebugUtilsMessageTypeFlagsEXT type,
    const vk::DebugUtilsMessengerCallbackDataEXT *pCallbackData, void *) {
    if (severity == vk::DebugUtilsMessageSeverityFlagBitsEXT::eError ||
        severity == vk::DebugUtilsMessageSeverityFlagBitsEXT::eWarning) {
        std::cerr << "[Vulkan - Validation Layer] Type: " << to_string(type)
                  << ", Message: " << pCallbackData->pMessage << std::endl;
    }

    return vk::False;
}

void VulkanInstance::createInstance() {
    constexpr vk::ApplicationInfo appInfo =
        vk::ApplicationInfo()
            .setPApplicationName("Game")
            .setApplicationVersion(vk::makeVersion(1, 0, 0))
            .setPEngineName("Avenir Engine")
            .setEngineVersion(vk::makeVersion(1, 0, 0))
            .setApiVersion(vk::ApiVersion14);

    const auto requiredLayers = findRequiredInstanceLayers();
    const auto requiredExtensions = findRequiredInstanceExtensions();

    const vk::InstanceCreateInfo instanceCreateInfo =
        vk::InstanceCreateInfo()
#ifdef __APPLE__
            .setFlags(vk::InstanceCreateFlagBits::eEnumeratePortabilityKHR)
#endif
            .setPApplicationInfo(&appInfo)
            .setEnabledLayerCount(static_cast<uint32_t>(requiredLayers.size()))
            .setPpEnabledLayerNames(requiredLayers.data())
            .setEnabledExtensionCount(
                static_cast<uint32_t>(requiredExtensions.size()))
            .setPpEnabledExtensionNames(requiredExtensions.data());

    m_instance = vk::raii::Instance(m_context, instanceCreateInfo);

    Debug::log("[Vulkan] Created: Instance",
               Debug::MessageSeverity::eInformation);
}

void VulkanInstance::setupDebugMessenger() {
    if constexpr (!m_shouldUseValidationLayers) {
        return;
    }

    constexpr vk::DebugUtilsMessageSeverityFlagsEXT severityFlags(
        vk::DebugUtilsMessageSeverityFlagBitsEXT::eVerbose |
        vk::DebugUtilsMessageSeverityFlagBitsEXT::eWarning |
        vk::DebugUtilsMessageSeverityFlagBitsEXT::eError);

    constexpr vk::DebugUtilsMessageTypeFlagsEXT messageTypeFlags(
        vk::DebugUtilsMessageTypeFlagBitsEXT::eGeneral |
        vk::DebugUtilsMessageTypeFlagBitsEXT::ePerformance |
        vk::DebugUtilsMessageTypeFlagBitsEXT::eValidation);

    const vk::DebugUtilsMessengerCreateInfoEXT createInfo =
        vk::DebugUtilsMessengerCreateInfoEXT()
            .setMessageSeverity(severityFlags)
            .setMessageType(messageTypeFlags)
            .setPfnUserCallback(
                reinterpret_cast<vk::PFN_DebugUtilsMessengerCallbackEXT>(
                    &debugCallback));

    m_debugMessenger = m_instance.createDebugUtilsMessengerEXT(createInfo);

    Debug::log("[Vulkan] Created: DebugUtilsMessenger",
               Debug::MessageSeverity::eInformation);
}

}  // namespace avenir::graphics::vulkan