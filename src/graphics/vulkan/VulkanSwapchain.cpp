#include "avenir/graphics/vulkan/VulkanSwapchain.hpp"

#include "avenir/debug/Debug.hpp"

namespace avenir::graphics::vulkan {

VulkanSwapchain::VulkanSwapchain(GLFWwindow *window,
                                 const vk::raii::SurfaceKHR &surface,
                                 const vk::raii::PhysicalDevice &physicalDevice,
                                 const vk::raii::Device &device)
    : m_glfwWindow(window),
      m_surface(surface),
      m_physicalDevice(physicalDevice),
      m_device(device) {
    createSwapchain();
    Debug::log("[Vulkan] Created: Swapchain",
               Debug::MessageSeverity::eInformation);

    createImageViews();
    Debug::log("[Vulkan] Created: ImageViews",
               Debug::MessageSeverity::eInformation);
}

void VulkanSwapchain::recreate() { recreateSwapchain(); }

const vk::raii::SwapchainKHR &VulkanSwapchain::handle() const {
    return m_swapchain;
}

const std::vector<vk::raii::ImageView> &VulkanSwapchain::imageViews() const {
    return m_swapchainImageViews;
}

const vk::Extent2D &VulkanSwapchain::extent() const {
    return m_swapchainExtent;
}

const std::vector<vk::Image> &VulkanSwapchain::images() const {
    return m_swapchainImages;
}

const vk::SurfaceFormatKHR &VulkanSwapchain::surfaceFormat() const {
    return m_swapchainSurfaceFormat;
}

VulkanSwapchain::~VulkanSwapchain() { cleanupSwapchain(); }

void VulkanSwapchain::createSwapchain() {
    const auto surfaceCapabilities =
        m_physicalDevice.getSurfaceCapabilitiesKHR(*m_surface);
    m_swapchainExtent = chooseSwapExtent(surfaceCapabilities);
    m_swapchainSurfaceFormat = chooseSwapSurfaceFormat(
        m_physicalDevice.getSurfaceFormatsKHR(*m_surface));

    const vk::SwapchainCreateInfoKHR swapchainCreateInfo =
        vk::SwapchainCreateInfoKHR()
            .setSurface(*m_surface)
            .setMinImageCount(chooseSwapMinImageCount(surfaceCapabilities))
            .setImageFormat(m_swapchainSurfaceFormat.format)
            .setImageColorSpace(m_swapchainSurfaceFormat.colorSpace)
            .setImageExtent(m_swapchainExtent)
            .setImageArrayLayers(1)
            .setImageUsage(vk::ImageUsageFlagBits::eColorAttachment)
            .setImageSharingMode(vk::SharingMode::eExclusive)
            .setPreTransform(surfaceCapabilities.currentTransform)
            .setCompositeAlpha(vk::CompositeAlphaFlagBitsKHR::eOpaque)
            .setPresentMode(chooseSwapPresentMode(
                m_physicalDevice.getSurfacePresentModesKHR(*m_surface)))
            .setClipped(vk::True);

    m_swapchain = vk::raii::SwapchainKHR(m_device, swapchainCreateInfo);
    m_swapchainImages = m_swapchain.getImages();
}

void VulkanSwapchain::recreateSwapchain() {
    m_device.waitIdle();

    cleanupSwapchain();
    createSwapchain();
    createImageViews();
}

void VulkanSwapchain::cleanupSwapchain() {
    m_swapchainImageViews.clear();
    m_swapchain = nullptr;
}

void VulkanSwapchain::createImageViews() {
    assert(m_swapchainImageViews.empty());

    vk::ImageViewCreateInfo imageViewCreateInfo =
        vk::ImageViewCreateInfo()
            .setViewType(vk::ImageViewType::e2D)
            .setFormat(m_swapchainSurfaceFormat.format)
            .setSubresourceRange(vk::ImageSubresourceRange(
                vk::ImageAspectFlagBits::eColor, 0, 1, 0, 1));

    for (const auto image : m_swapchainImages) {
        imageViewCreateInfo.setImage(image);
        m_swapchainImageViews.emplace_back(m_device, imageViewCreateInfo);
    }
}

vk::Extent2D VulkanSwapchain::chooseSwapExtent(
    const vk::SurfaceCapabilitiesKHR &capabilities) const {
    if (capabilities.currentExtent.width !=
        std::numeric_limits<uint32_t>::max()) {
        return capabilities.currentExtent;
    }

    int width;
    int height;
    glfwGetFramebufferSize(m_glfwWindow, &width, &height);
    return {std::clamp<uint32_t>(width, capabilities.minImageExtent.width,
                                 capabilities.maxImageExtent.width),
            std::clamp<uint32_t>(height, capabilities.minImageExtent.height,
                                 capabilities.maxImageExtent.height)};
}

vk::SurfaceFormatKHR VulkanSwapchain::chooseSwapSurfaceFormat(
    const std::vector<vk::SurfaceFormatKHR> &availableFormats) {
    assert(!availableFormats.empty());
    const auto formatIterator =
        std::ranges::find_if(availableFormats, [](const auto &format) {
            return format.format == vk::Format::eB8G8R8A8Srgb &&
                   format.colorSpace == vk::ColorSpaceKHR::eSrgbNonlinear;
        });

    return formatIterator != availableFormats.end() ? *formatIterator
                                                    : availableFormats[0];
}

uint32_t VulkanSwapchain::chooseSwapMinImageCount(
    const vk::SurfaceCapabilitiesKHR &surfaceCapabilities) {
    auto minImageCount = std::max(3u, surfaceCapabilities.minImageCount);
    if ((0 < surfaceCapabilities.maxImageCount) &&
        (surfaceCapabilities.maxImageCount < minImageCount)) {
        minImageCount = surfaceCapabilities.maxImageCount;
    }

    return minImageCount;
}

vk::PresentModeKHR VulkanSwapchain::chooseSwapPresentMode(
    const std::vector<vk::PresentModeKHR> &availablePresentModes) {
    assert(std::ranges::any_of(availablePresentModes, [](auto presentMode) {
        return presentMode == vk::PresentModeKHR::eFifo;
    }));

    return std::ranges::any_of(availablePresentModes,
                               [](const vk::PresentModeKHR value) {
                                   return vk::PresentModeKHR::eMailbox == value;
                               })
               ? vk::PresentModeKHR::eMailbox
               : vk::PresentModeKHR::eFifo;
}

}  // namespace avenir::graphics::vulkan