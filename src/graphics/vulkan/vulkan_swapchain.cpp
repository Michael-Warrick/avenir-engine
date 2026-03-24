#include "avenir/graphics/vulkan/vulkan_swapchain.hpp"

#include "avenir/debug/debug.hpp"

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
    debug::log("[Vulkan] Created: Swapchain",
               debug::MessageSeverity::eInformation);

    createImageViews();
    debug::log("[Vulkan] Created: Swapchain ImageViews",
               debug::MessageSeverity::eInformation);

    createDepthResources();
    debug::log("[Vulkan] Created: Swapchain Depth Resources",
               debug::MessageSeverity::eInformation);
}

VulkanSwapchain::~VulkanSwapchain() { cleanupSwapchain(); }

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

const vk::raii::Image &VulkanSwapchain::depthImage() const {
    return m_depthImage;
}

const vk::raii::ImageView &VulkanSwapchain::depthImageView() const {
    return m_depthImageView;
}

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
    createDepthResources();
}

void VulkanSwapchain::cleanupSwapchain() {
    m_swapchainImageViews.clear();
    m_swapchain = nullptr;
}

void VulkanSwapchain::createDepthResources() {
    const vk::Format depthFormat = findDepthFormat();
    createImage(m_swapchainExtent.width, m_swapchainExtent.height, depthFormat,
                vk::ImageTiling::eOptimal,
                vk::ImageUsageFlagBits::eDepthStencilAttachment,
                vk::MemoryPropertyFlagBits::eDeviceLocal, m_depthImage,
                m_depthImageMemory);
    m_depthImageView = createImageView(m_depthImage, depthFormat,
                                       vk::ImageAspectFlagBits::eDepth);
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

vk::raii::ImageView VulkanSwapchain::createImageView(
    const vk::raii::Image &image, const vk::Format format,
    const vk::ImageAspectFlags aspectFlags) const {
    const vk::ImageViewCreateInfo viewInfo =
        vk::ImageViewCreateInfo()
            .setImage(image)
            .setViewType(vk::ImageViewType::e2D)
            .setFormat(format)
            .setSubresourceRange(
                vk::ImageSubresourceRange(aspectFlags, 0, 1, 0, 1));

    return vk::raii::ImageView(m_device, viewInfo);
}

void VulkanSwapchain::createImage(const uint32_t width, const uint32_t height,
                                  vk::Format format, vk::ImageTiling tiling,
                                  const vk::ImageUsageFlags usage,
                                  const vk::MemoryPropertyFlags properties,
                                  vk::raii::Image &image,
                                  vk::raii::DeviceMemory &imageMemory) const {
    const vk::ImageCreateInfo imageInfo =
        vk::ImageCreateInfo()
            .setImageType(vk::ImageType::e2D)
            .setFormat(format)
            .setExtent(vk::Extent3D(width, height, 1))
            .setMipLevels(1)
            .setArrayLayers(1)
            .setSamples(vk::SampleCountFlagBits::e1)
            .setTiling(tiling)
            .setUsage(usage)
            .setSharingMode(vk::SharingMode::eExclusive);

    image = vk::raii::Image(m_device, imageInfo);

    const vk::MemoryRequirements memoryRequirements =
        image.getMemoryRequirements();
    const vk::MemoryAllocateInfo memoryAllocInfo =
        vk::MemoryAllocateInfo()
            .setAllocationSize(memoryRequirements.size)
            .setMemoryTypeIndex(
                findMemoryType(memoryRequirements.memoryTypeBits, properties));

    imageMemory = vk::raii::DeviceMemory(m_device, memoryAllocInfo);
    image.bindMemory(imageMemory, 0);
}

uint32_t VulkanSwapchain::findMemoryType(
    const uint32_t typeFilter, const vk::MemoryPropertyFlags properties) const {
    const vk::PhysicalDeviceMemoryProperties memoryProperties =
        m_physicalDevice.getMemoryProperties();
    for (uint32_t i = 0; i < memoryProperties.memoryTypeCount; ++i) {
        // If there is a memory type suitable for the buffer that also has all
        // the properties we need, then we return its index
        if ((typeFilter & (1 << i)) &&
            (memoryProperties.memoryTypes[i].propertyFlags & properties) ==
                properties) {
            return i;
        }
    }

    throw std::runtime_error(
        "[Vulkan] Error: Failed to find suitable memory type!\n");
}

vk::Format VulkanSwapchain::findDepthFormat() {
    return findSupportedFormat(
        {vk::Format::eD32Sfloat, vk::Format::eD32SfloatS8Uint,
         vk::Format::eD24UnormS8Uint},
        vk::ImageTiling::eOptimal,
        vk::FormatFeatureFlagBits::eDepthStencilAttachment);
}

vk::Format VulkanSwapchain::findSupportedFormat(
    const std::vector<vk::Format> &candidates, const vk::ImageTiling tiling,
    const vk::FormatFeatureFlags features) {
    const auto formatIterator =
        std::ranges::find_if(candidates, [&](auto const format) {
            vk::FormatProperties properties =
                m_physicalDevice.getFormatProperties(format);

            return (
                ((tiling == vk::ImageTiling::eLinear) &&
                 ((properties.linearTilingFeatures & features) == features)) ||
                ((tiling == vk::ImageTiling::eOptimal) &&
                 ((properties.optimalTilingFeatures & features) == features)));
        });

    if (formatIterator == candidates.end()) {
        throw std::runtime_error("[Vulkan] Failed to find supported format!");
    }

    return *formatIterator;
}

}  // namespace avenir::graphics::vulkan