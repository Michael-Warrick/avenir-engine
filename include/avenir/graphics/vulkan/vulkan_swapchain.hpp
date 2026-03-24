#ifndef AVENIR_GRAPHICS_VULKAN_VULKANSWAPCHAIN_HPP
#define AVENIR_GRAPHICS_VULKAN_VULKANSWAPCHAIN_HPP

#include <vulkan/vulkan_raii.hpp>

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

namespace avenir::graphics::vulkan {

class VulkanSwapchain {
public:
    VulkanSwapchain(GLFWwindow *window, const vk::raii::SurfaceKHR &surface,
                    const vk::raii::PhysicalDevice &physicalDevice,
                    const vk::raii::Device &device);
    ~VulkanSwapchain();

    void recreate();

    [[nodiscard]] const vk::raii::SwapchainKHR &handle() const;
    [[nodiscard]] const std::vector<vk::raii::ImageView> &imageViews() const;
    [[nodiscard]] const vk::Extent2D &extent() const;
    [[nodiscard]] const std::vector<vk::Image> &images() const;
    [[nodiscard]] const vk::SurfaceFormatKHR &surfaceFormat() const;
    [[nodiscard]] const vk::raii::Image &depthImage() const;
    [[nodiscard]] const vk::raii::ImageView &depthImageView() const;

private:
    void createSwapchain();
    void createImageViews();
    void createDepthResources();
    void recreateSwapchain();
    void cleanupSwapchain();

    [[nodiscard]] vk::Extent2D chooseSwapExtent(
        const vk::SurfaceCapabilitiesKHR &capabilities) const;

    static vk::SurfaceFormatKHR chooseSwapSurfaceFormat(
        const std::vector<vk::SurfaceFormatKHR> &availableFormats);

    static uint32_t chooseSwapMinImageCount(
        const vk::SurfaceCapabilitiesKHR &surfaceCapabilities);

    static vk::PresentModeKHR chooseSwapPresentMode(
        const std::vector<vk::PresentModeKHR> &availablePresentModes);

    void createImage(uint32_t width, uint32_t height, vk::Format format,
                     vk::ImageTiling tiling, vk::ImageUsageFlags usage,
                     vk::MemoryPropertyFlags properties, vk::raii::Image &image,
                     vk::raii::DeviceMemory &imageMemory) const;

    uint32_t findMemoryType(const uint32_t typeFilter,
                            const vk::MemoryPropertyFlags properties) const;

    vk::Format findDepthFormat();

    vk::Format findSupportedFormat(const std::vector<vk::Format> &candidates,
                                   vk::ImageTiling tiling,
                                   vk::FormatFeatureFlags features);

    vk::raii::ImageView createImageView(const vk::raii::Image &image,
                                        vk::Format format,
                                        vk::ImageAspectFlags aspectFlags) const;

    GLFWwindow *m_glfwWindow = nullptr;
    const vk::raii::SurfaceKHR &m_surface;
    const vk::raii::PhysicalDevice &m_physicalDevice;
    const vk::raii::Device &m_device;

    vk::raii::SwapchainKHR m_swapchain = nullptr;
    std::vector<vk::Image> m_swapchainImages;
    vk::SurfaceFormatKHR m_swapchainSurfaceFormat;
    vk::Extent2D m_swapchainExtent;
    std::vector<vk::raii::ImageView> m_swapchainImageViews;
    vk::raii::Image m_depthImage = nullptr;
    vk::raii::DeviceMemory m_depthImageMemory = nullptr;
    vk::raii::ImageView m_depthImageView = nullptr;
};

}  // namespace avenir::graphics::vulkan

#endif  // AVENIR_GRAPHICS_VULKAN_VULKANSWAPCHAIN_HPP