#ifndef AVENIR_GRAPHICS_VULKAN_VULKANINSTANCE_HPP
#define AVENIR_GRAPHICS_VULKAN_VULKANINSTANCE_HPP

#include <vulkan/vulkan_raii.hpp>

namespace avenir::graphics::vulkan {

class VulkanInstance {
public:
    VulkanInstance();
    ~VulkanInstance() = default;

    [[nodiscard]] const vk::raii::Instance &instance() const;

private:
    void printAllAvailableInstanceExtensions() const;

    [[nodiscard]] std::vector<const char *> findRequiredInstanceLayers() const;

    static std::vector<const char *> findRequiredInstanceExtensions();

    static VKAPI_ATTR vk::Bool32 VKAPI_CALL debugCallback(
        vk::DebugUtilsMessageSeverityFlagBitsEXT severity,
        vk::DebugUtilsMessageTypeFlagsEXT type,
        const vk::DebugUtilsMessengerCallbackDataEXT *pCallbackData, void *);

    void createInstance();

    void setupDebugMessenger();

    vk::raii::Context m_context;
    vk::raii::Instance m_instance = nullptr;
    vk::raii::DebugUtilsMessengerEXT m_debugMessenger = nullptr;

#ifdef NDEBUG
    static constexpr bool m_shouldUseValidationLayers = false;
#else
    static constexpr bool m_shouldUseValidationLayers = true;
#endif

    const std::vector<const char *> m_validationLayers = {
        "VK_LAYER_KHRONOS_validation"};
};

}  // namespace avenir::graphics::vulkan

#endif  // AVENIR_GRAPHICS_VULKAN_VULKANINSTANCE_HPP