#include "avenir/graphics/vulkan/VulkanRenderer.hpp"

#include <chrono>
#include <cstdint>
#include <fstream>
#include <iostream>
#include <string>
#include <vector>

#include "avenir/debug/Debug.hpp"

namespace avenir::graphics::vulkan {

VulkanRenderer::VulkanRenderer(GLFWwindow *window) : m_glfwWindow(window) {
    createInstance();
    setupDebugMessenger();
    createSurface();
    pickPhysicalDevice();
    createLogicalDevice();
    createSwapchain();
    createImageViews();
    createDescriptorSetLayout();
    createGraphicsPipeline();
    createCommandPool();
    createTextureImage();
    createTextureImageView();
    createTextureSampler();
    createVertexBuffer();
    createIndexBuffer();
    createUniformBuffers();
    createDescriptorPool();
    createDescriptorSets();
    createCommandBuffers();
    createSyncObjects();

    m_isFirstRun = false;

    std::cout << "---------------------------------------------------\n";
    Debug::log("[Vulkan] Successfully initiated, now rendering...",
               Debug::MessageSeverity::eInformation);
}

VulkanRenderer::~VulkanRenderer() {
    m_logicalDevice.waitIdle();

    cleanupSwapchain();

    std::cout << "---------------------------------------------------\n";
    Debug::log("[Vulkan] Shutting down...",
               Debug::MessageSeverity::eInformation);
}

void VulkanRenderer::drawFrame(const glm::mat4 cameraViewMatrix) {
    while (vk::Result::eTimeout ==
           m_logicalDevice.waitForFences(*m_inFlightFences[m_currentFrame],
                                         vk::True, UINT64_MAX)) {
        ;
    }

    auto [result, imageIndex] = m_swapchain.acquireNextImage(
        UINT64_MAX, *m_presentCompleteSemaphores[m_semaphoreIndex], nullptr);

    if (result == vk::Result::eErrorOutOfDateKHR) {
        recreateSwapchain();
        return;
    }

    if (result != vk::Result::eSuccess &&
        result != vk::Result::eSuboptimalKHR) {
        throw std::runtime_error(
            "[Vulkan] Error: Failed to acquire swapchain image!\n");
    }

    updateUniformBuffer(m_currentFrame, cameraViewMatrix);

    m_logicalDevice.resetFences(*m_inFlightFences[m_currentFrame]);

    m_commandBuffers[m_currentFrame].reset();
    recordCommandBuffer(imageIndex);

    constexpr vk::PipelineStageFlags waitDestinationStageMask(
        vk::PipelineStageFlagBits::eColorAttachmentOutput);
    const vk::SubmitInfo submitInfo =
        vk::SubmitInfo()
            .setWaitSemaphoreCount(1)
            .setPWaitSemaphores(&*m_presentCompleteSemaphores[m_semaphoreIndex])
            .setPWaitDstStageMask(&waitDestinationStageMask)
            .setCommandBufferCount(1)
            .setPCommandBuffers(&*m_commandBuffers[m_currentFrame])
            .setSignalSemaphoreCount(1)
            .setPSignalSemaphores(&*m_renderFinishedSemaphores[imageIndex]);

    m_queue.submit(submitInfo, *m_inFlightFences[m_currentFrame]);

    try {
        const vk::PresentInfoKHR presentInfoKHR =
            vk::PresentInfoKHR()
                .setWaitSemaphoreCount(1)
                .setPWaitSemaphores(&*m_renderFinishedSemaphores[imageIndex])
                .setSwapchainCount(1)
                .setPSwapchains(&*m_swapchain)
                .setPImageIndices(&imageIndex);

        result = m_queue.presentKHR(presentInfoKHR);
        if (result == vk::Result::eErrorOutOfDateKHR ||
            result == vk::Result::eSuboptimalKHR || m_framebufferResized) {
            m_framebufferResized = false;
            recreateSwapchain();
        } else if (result != vk::Result::eSuccess) {
            throw std::runtime_error(
                "[Vulkan] Error: Failed to present swapchain image!\n");
        }

    } catch (const vk::SystemError &e) {
        if (e.code().value() ==
            static_cast<int>(vk::Result::eErrorOutOfDateKHR)) {
            recreateSwapchain();
            return;
        } else {
            throw;
        }
    }

    m_semaphoreIndex =
        (m_semaphoreIndex + 1) % m_presentCompleteSemaphores.size();
    m_currentFrame = (m_currentFrame + 1) % m_kFramesInFlight;
}

void VulkanRenderer::onFramebufferResize(int width, int height) {
    m_framebufferResized = true;
}

void VulkanRenderer::printAllAvailableInstanceExtensions() const {
    Debug::log("[Vulkan] Available instance extensions:",
               Debug::MessageSeverity::eInformation);

    auto extensions = m_context.enumerateInstanceExtensionProperties();
    for (const auto &extension : extensions) {
        std::string extensionName =
            "\t\t" + static_cast<std::string>(extension.extensionName.data());
        Debug::log(extensionName, Debug::MessageSeverity::eInformation);
    }
}

std::vector<const char *> VulkanRenderer::findRequiredInstanceLayers() const {
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

std::vector<const char *> VulkanRenderer::findRequiredInstanceExtensions() {
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

VKAPI_ATTR vk::Bool32 VKAPI_CALL VulkanRenderer::debugCallback(
    vk::DebugUtilsMessageSeverityFlagBitsEXT severity,
    vk::DebugUtilsMessageTypeFlagsEXT type,
    const vk::DebugUtilsMessengerCallbackDataEXT *pCallbackData, void *) {
    if (severity == vk::DebugUtilsMessageSeverityFlagBitsEXT::eError ||
        severity == vk::DebugUtilsMessageSeverityFlagBitsEXT::eWarning) {
        std::cerr << "[Vulkan - Validation Layer] Type: " << to_string(type)
                  << ", Message: " << pCallbackData->pMessage << std::endl;
    }

    return vk::False;
}

uint32_t VulkanRenderer::chooseSwapMinImageCount(
    const vk::SurfaceCapabilitiesKHR &surfaceCapabilities) {
    auto minImageCount = std::max(3u, surfaceCapabilities.minImageCount);
    if ((0 < surfaceCapabilities.maxImageCount) &&
        (surfaceCapabilities.maxImageCount < minImageCount)) {
        minImageCount = surfaceCapabilities.maxImageCount;
    }

    return minImageCount;
}

vk::SurfaceFormatKHR VulkanRenderer::chooseSwapSurfaceFormat(
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

vk::PresentModeKHR VulkanRenderer::chooseSwapPresentMode(
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

vk::Extent2D VulkanRenderer::chooseSwapExtent(
    const vk::SurfaceCapabilitiesKHR &capabilities) {
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

vk::raii::ShaderModule VulkanRenderer::createShaderModule(
    const std::vector<char> &code) const {
    vk::ShaderModuleCreateInfo shaderModuleCreateInfo =
        vk::ShaderModuleCreateInfo()
            .setCodeSize(code.size() * sizeof(char))
            .setPCode(reinterpret_cast<const uint32_t *>(code.data()));

    vk::raii::ShaderModule shaderModule(m_logicalDevice,
                                        shaderModuleCreateInfo);
    return shaderModule;
}

void VulkanRenderer::recordCommandBuffer(uint32_t imageIndex) {
    m_commandBuffers[m_currentFrame].begin({});

    // Before rendering, transition the swapchain image to
    // `eColorAttachmentOptimal`
    transitionImageLayout(imageIndex, vk::ImageLayout::eUndefined,
                          vk::ImageLayout::eColorAttachmentOptimal, {},
                          vk::AccessFlagBits2::eColorAttachmentWrite,
                          vk::PipelineStageFlagBits2::eColorAttachmentOutput,
                          vk::PipelineStageFlagBits2::eColorAttachmentOutput);

    vk::ClearValue clearColor = vk::ClearColorValue(0.529, 0.807, 0.921, 1.0f);
    vk::RenderingAttachmentInfo attachmentInfo =
        vk::RenderingAttachmentInfo()
            .setImageView(m_swapchainImageViews[imageIndex])
            .setImageLayout(vk::ImageLayout::eColorAttachmentOptimal)
            .setLoadOp(vk::AttachmentLoadOp::eClear)
            .setStoreOp(vk::AttachmentStoreOp::eStore)
            .setClearValue(clearColor);

    vk::RenderingInfo renderingInfo =
        vk::RenderingInfo()
            .setRenderArea(vk::Rect2D(vk::Offset2D(0, 0), m_swapchainExtent))
            .setLayerCount(1)
            .setColorAttachmentCount(1)
            .setPColorAttachments(&attachmentInfo);

    m_commandBuffers[m_currentFrame].beginRendering(renderingInfo);
    {
        m_commandBuffers[m_currentFrame].bindPipeline(
            vk::PipelineBindPoint::eGraphics, m_graphicsPipeline);

        m_commandBuffers[m_currentFrame].setViewport(
            0, vk::Viewport(
                   0.0f, 0.0f, static_cast<float>(m_swapchainExtent.width),
                   static_cast<float>(m_swapchainExtent.height), 0.0f, 1.0f));
        m_commandBuffers[m_currentFrame].setScissor(
            0, vk::Rect2D(vk::Offset2D(0, 0), m_swapchainExtent));

        m_commandBuffers[m_currentFrame].bindVertexBuffers(0, *m_vertexBuffer,
                                                           {0});
        m_commandBuffers[m_currentFrame].bindIndexBuffer(
            *m_indexBuffer, 0, vk::IndexType::eUint16);
        m_commandBuffers[m_currentFrame].bindDescriptorSets(
            vk::PipelineBindPoint::eGraphics, m_pipelineLayout, 0,
            *m_descriptorSets[m_currentFrame], nullptr);

        m_commandBuffers[m_currentFrame].drawIndexed(
            static_cast<uint16_t>(m_indices.size()), 1, 0, 0, 0);
    }

    m_commandBuffers[m_currentFrame].endRendering();

    // After rendering, transition the swapchain image to `ePresentSrcKHR`
    transitionImageLayout(imageIndex, vk::ImageLayout::eColorAttachmentOptimal,
                          vk::ImageLayout::ePresentSrcKHR,
                          vk::AccessFlagBits2::eColorAttachmentWrite, {},
                          vk::PipelineStageFlagBits2::eColorAttachmentOutput,
                          vk::PipelineStageFlagBits2::eBottomOfPipe);

    m_commandBuffers[m_currentFrame].end();
}

void VulkanRenderer::transitionImageLayout(const vk::raii::Image &image,
                                           vk::ImageLayout oldLayout,
                                           vk::ImageLayout newLayout) {
    vk::raii::CommandBuffer commandBuffer = beginSingleTimeCommands();

    vk::ImageMemoryBarrier barrier =
        vk::ImageMemoryBarrier()
            .setOldLayout(oldLayout)
            .setNewLayout(newLayout)
            .setImage(image)
            .setSubresourceRange(vk::ImageSubresourceRange(
                vk::ImageAspectFlagBits::eColor, 0, 1, 0, 1));

    vk::PipelineStageFlags sourceStage;
    vk::PipelineStageFlags destinationStage;

    if (oldLayout == vk::ImageLayout::eUndefined &&
        newLayout == vk::ImageLayout::eTransferDstOptimal) {
        barrier.setSrcAccessMask({});
        barrier.setDstAccessMask(vk::AccessFlagBits::eTransferWrite);

        sourceStage = vk::PipelineStageFlagBits::eTopOfPipe;
        destinationStage = vk::PipelineStageFlagBits::eTransfer;
    } else if (oldLayout == vk::ImageLayout::eTransferDstOptimal &&
               newLayout == vk::ImageLayout::eShaderReadOnlyOptimal) {
        barrier.setSrcAccessMask(vk::AccessFlagBits::eTransferWrite);
        barrier.setDstAccessMask(vk::AccessFlagBits::eShaderRead);

        sourceStage = vk::PipelineStageFlagBits::eTransfer;
        destinationStage = vk::PipelineStageFlagBits::eFragmentShader;
    } else {
        throw std::runtime_error("Error: Unsupported layout transition!\n");
    }

    commandBuffer.pipelineBarrier(sourceStage, destinationStage, {}, {},
                                  nullptr, barrier);
    endSingleTimeCommands(commandBuffer);
}

void VulkanRenderer::transitionImageLayout(
    uint32_t imageIndex, vk::ImageLayout oldLayout, vk::ImageLayout newLayout,
    vk::AccessFlags2 sourceAccessMask, vk::AccessFlags2 destinationAccessMask,
    vk::PipelineStageFlags2 sourceStageMask,
    vk::PipelineStageFlags2 destinationStageMask) {
    vk::ImageMemoryBarrier2 barrier =
        vk::ImageMemoryBarrier2()
            .setSrcStageMask(sourceStageMask)
            .setSrcAccessMask(sourceAccessMask)
            .setDstStageMask(destinationStageMask)
            .setDstAccessMask(destinationAccessMask)
            .setOldLayout(oldLayout)
            .setNewLayout(newLayout)
            .setSrcQueueFamilyIndex(vk::QueueFamilyIgnored)
            .setDstQueueFamilyIndex(vk::QueueFamilyIgnored)
            .setImage(m_swapchainImages[imageIndex])
            .setSubresourceRange(vk::ImageSubresourceRange(
                /* aspectMask */ vk::ImageAspectFlagBits::eColor,
                /* baseMipLevel */ 0,
                /* levelCount */ 1,
                /* baseArrayLayer */ 0,
                /* layerCount */ 1));

    vk::DependencyInfo dependencyInfo = vk::DependencyInfo()
                                            .setDependencyFlags({})
                                            .setImageMemoryBarrierCount(1)
                                            .setPImageMemoryBarriers(&barrier);

    m_commandBuffers[m_currentFrame].pipelineBarrier2(dependencyInfo);
}

void VulkanRenderer::cleanupSwapchain() {
    m_swapchainImageViews.clear();
    m_swapchain = nullptr;
}

void VulkanRenderer::recreateSwapchain() {
    m_logicalDevice.waitIdle();

    cleanupSwapchain();
    createSwapchain();
    createImageViews();
}

uint32_t VulkanRenderer::findMemoryType(uint32_t typeFilter,
                                        vk::MemoryPropertyFlags properties) {
    vk::PhysicalDeviceMemoryProperties memoryProperties =
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

void VulkanRenderer::createBuffer(vk::DeviceSize size,
                                  vk::BufferUsageFlags usage,
                                  vk::MemoryPropertyFlags properties,
                                  vk::raii::Buffer &buffer,
                                  vk::raii::DeviceMemory &bufferMemory) {
    vk::BufferCreateInfo bufferInfo =
        vk::BufferCreateInfo().setSize(size).setUsage(usage).setSharingMode(
            vk::SharingMode::eExclusive);

    buffer = vk::raii::Buffer(m_logicalDevice, bufferInfo);

    vk::MemoryRequirements memoryRequirements = buffer.getMemoryRequirements();
    vk::MemoryAllocateInfo memoryAllocateInfo =
        vk::MemoryAllocateInfo()
            .setAllocationSize(memoryRequirements.size)
            .setMemoryTypeIndex(
                findMemoryType(memoryRequirements.memoryTypeBits, properties));

    bufferMemory = vk::raii::DeviceMemory(m_logicalDevice, memoryAllocateInfo);
    buffer.bindMemory(bufferMemory, 0);
}

void VulkanRenderer::copyBuffer(const vk::raii::Buffer &sourceBuffer,
                                const vk::raii::Buffer &destinationBuffer,
                                const vk::DeviceSize size) const {
    const vk::raii::CommandBuffer commandCopyBuffer = beginSingleTimeCommands();
    commandCopyBuffer.copyBuffer(sourceBuffer, destinationBuffer,
                                 vk::BufferCopy(0, 0, size));
    endSingleTimeCommands(commandCopyBuffer);
}

void VulkanRenderer::copyBufferToImage(const vk::raii::Buffer &buffer,
                                       const vk::raii::Image &image,
                                       const uint32_t width,
                                       const uint32_t height) const {
    const vk::raii::CommandBuffer commandBuffer = beginSingleTimeCommands();

    vk::BufferImageCopy region =
        vk::BufferImageCopy()
            .setBufferOffset(0)
            .setBufferRowLength(0)
            .setBufferImageHeight(0)
            .setImageSubresource(vk::ImageSubresourceLayers(
                vk::ImageAspectFlagBits::eColor, 0, 0, 1))
            .setImageOffset(vk::Offset3D(0, 0, 0))
            .setImageExtent(vk::Extent3D(width, height, 1));

    commandBuffer.copyBufferToImage(
        buffer, image, vk::ImageLayout::eTransferDstOptimal, {region});

    endSingleTimeCommands(commandBuffer);
}

void VulkanRenderer::updateUniformBuffer(const uint32_t currentImage,
                                         const glm::mat4 &viewMatrix) const {
    UniformBufferObject ubo{};
    ubo.model = glm::mat4(1.0f);
    ubo.view = viewMatrix;

    ubo.projection =
        glm::perspective(glm::radians(45.0f),
                         static_cast<float>(m_swapchainExtent.width) /
                             static_cast<float>(m_swapchainExtent.height),
                         0.1f, 10.f);

    // Flipping Y coordinate of clip coordinates to match Vulkan's
    ubo.projection[1][1] *= -1;

    memcpy(m_uniformBuffersMapped[currentImage], &ubo, sizeof(ubo));
}

std::vector<char> VulkanRenderer::readFile(const std::string &fileName) {
    std::ifstream file(fileName, std::ios::ate | std::ios::binary);
    if (!file.is_open()) {
        throw std::runtime_error("Error: Failed to open file!\n");
    }

    std::vector<char> buffer(file.tellg());

    file.seekg(0, std::ios::beg);
    file.read(buffer.data(), static_cast<std::streamsize>(buffer.size()));
    file.close();

    return buffer;
}

void VulkanRenderer::createImage(const uint32_t width, const uint32_t height,
                                 vk::Format format, vk::ImageTiling tiling,
                                 const vk::ImageUsageFlags usage,
                                 const vk::MemoryPropertyFlags properties,
                                 vk::raii::Image &image,
                                 vk::raii::DeviceMemory &imageMemory) {
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

    image = vk::raii::Image(m_logicalDevice, imageInfo);

    const vk::MemoryRequirements memoryRequirements =
        image.getMemoryRequirements();
    const vk::MemoryAllocateInfo memoryAllocInfo =
        vk::MemoryAllocateInfo()
            .setAllocationSize(memoryRequirements.size)
            .setMemoryTypeIndex(
                findMemoryType(memoryRequirements.memoryTypeBits, properties));

    imageMemory = vk::raii::DeviceMemory(m_logicalDevice, memoryAllocInfo);
    image.bindMemory(imageMemory, 0);
}

vk::raii::CommandBuffer VulkanRenderer::beginSingleTimeCommands() const {
    const vk::CommandBufferAllocateInfo allocInfo =
        vk::CommandBufferAllocateInfo()
            .setCommandPool(m_commandPool)
            .setLevel(vk::CommandBufferLevel::ePrimary)
            .setCommandBufferCount(1);

    vk::raii::CommandBuffer commandBuffer =
        std::move(m_logicalDevice.allocateCommandBuffers(allocInfo).front());

    constexpr vk::CommandBufferBeginInfo beginInfo =
        vk::CommandBufferBeginInfo().setFlags(
            vk::CommandBufferUsageFlagBits::eOneTimeSubmit);

    commandBuffer.begin(beginInfo);

    return commandBuffer;
}

void VulkanRenderer::endSingleTimeCommands(
    const vk::raii::CommandBuffer &commandBuffer) const {
    commandBuffer.end();

    const vk::SubmitInfo submitInfo =
        vk::SubmitInfo().setCommandBufferCount(1).setPCommandBuffers(
            &*commandBuffer);
    m_queue.submit(submitInfo, nullptr);
    m_queue.waitIdle();
}

vk::raii::ImageView VulkanRenderer::createImageView(vk::raii::Image &image,
                                                    vk::Format format) {
    vk::ImageViewCreateInfo viewInfo =
        vk::ImageViewCreateInfo()
            .setImage(image)
            .setViewType(vk::ImageViewType::e2D)
            .setFormat(format)
            .setSubresourceRange(vk::ImageSubresourceRange(
                vk::ImageAspectFlagBits::eColor, 0, 1, 0, 1));

    return vk::raii::ImageView(m_logicalDevice, viewInfo);
}

void VulkanRenderer::createInstance() {
    constexpr vk::ApplicationInfo appInfo =
        vk::ApplicationInfo()
            .setPApplicationName("LearningVulkan")
            .setApplicationVersion(vk::makeVersion(1, 0, 0))
            .setPEngineName("No Engine")
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

void VulkanRenderer::setupDebugMessenger() {
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

void VulkanRenderer::createSurface() {
    VkSurfaceKHR surface;
    if (glfwCreateWindowSurface(*m_instance, m_glfwWindow, nullptr, &surface) !=
        VK_SUCCESS) {
        throw std::runtime_error(
            "[Vulkan] Error: Failed to create window surface!");
    }

    m_surface = vk::raii::SurfaceKHR(m_instance, surface);

    Debug::log("[Vulkan] Created: Surface",
               Debug::MessageSeverity::eInformation);
}

void VulkanRenderer::pickPhysicalDevice() {
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
            bool supportsGraphicsOperations = std::ranges::any_of(
                queueFamilies, [](auto const &queueFamilyProperties) {
                    return !!(queueFamilyProperties.queueFlags &
                              vk::QueueFlagBits::eGraphics);
                });

            // Check all required physical device extensions are available
            auto availableExtensions =
                physicalDevice.enumerateDeviceExtensionProperties();
            bool supportsAllRequiredExtensions = std::ranges::all_of(
                m_deviceExtensions,
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

            bool supportsRequiredFeatures =
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
            "[Vulkan] Error: Failed to find a suitible GPU!\n");
    }

    Debug::log("[Vulkan] Created: PhysicalDevice",
               Debug::MessageSeverity::eInformation);
}

void VulkanRenderer::createLogicalDevice() {
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
                static_cast<uint32_t>(m_deviceExtensions.size()))
            .setPpEnabledExtensionNames(m_deviceExtensions.data());

    m_logicalDevice =
        vk::raii::Device(m_physicalDevice, logicalDeviceCreateInfo);
    m_queue = vk::raii::Queue(m_logicalDevice, m_queueIndex, 0);

    Debug::log("[Vulkan] Created: Device",
               Debug::MessageSeverity::eInformation);
    Debug::log("[Vulkan] Created: Queue (Graphics and Presentation)",
               Debug::MessageSeverity::eInformation);
}

void VulkanRenderer::createSwapchain() {
    auto surfaceCapabilities =
        m_physicalDevice.getSurfaceCapabilitiesKHR(*m_surface);
    m_swapchainExtent = chooseSwapExtent(surfaceCapabilities);
    m_swapchainSurfaceFormat = chooseSwapSurfaceFormat(
        m_physicalDevice.getSurfaceFormatsKHR(*m_surface));

    vk::SwapchainCreateInfoKHR swapchainCreateinfo =
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

    m_swapchain = vk::raii::SwapchainKHR(m_logicalDevice, swapchainCreateinfo);
    m_swapchainImages = m_swapchain.getImages();

    if (m_isFirstRun) {
        Debug::log("[Vulkan] Created: Swapchain",
                   Debug::MessageSeverity::eInformation);
    }
}

void VulkanRenderer::createImageViews() {
    assert(m_swapchainImageViews.empty());

    vk::ImageViewCreateInfo imageViewCreateInfo =
        vk::ImageViewCreateInfo()
            .setViewType(vk::ImageViewType::e2D)
            .setFormat(m_swapchainSurfaceFormat.format)
            .setSubresourceRange(vk::ImageSubresourceRange(
                vk::ImageAspectFlagBits::eColor, 0, 1, 0, 1));

    for (const auto image : m_swapchainImages) {
        imageViewCreateInfo.setImage(image);
        m_swapchainImageViews.emplace_back(m_logicalDevice,
                                           imageViewCreateInfo);
    }

    if (m_isFirstRun) {
        Debug::log("[Vulkan] Created: ImageViews",
                   Debug::MessageSeverity::eInformation);
    }
}

void VulkanRenderer::createDescriptorSetLayout() {
    std::array<vk::DescriptorSetLayoutBinding, 2> bindings = {
        vk::DescriptorSetLayoutBinding(0, vk::DescriptorType::eUniformBuffer, 1,
                                       vk::ShaderStageFlagBits::eVertex,
                                       nullptr),
        vk::DescriptorSetLayoutBinding(
            1, vk::DescriptorType::eCombinedImageSampler, 1,
            vk::ShaderStageFlagBits::eFragment, nullptr)};

    const vk::DescriptorSetLayoutCreateInfo layoutInfo =
        vk::DescriptorSetLayoutCreateInfo()
            .setBindingCount(bindings.size())
            .setPBindings(bindings.data());

    m_descriptorSetLayout =
        vk::raii::DescriptorSetLayout(m_logicalDevice, layoutInfo);

    Debug::log("[Vulkan] Created: DescriptorSetLayout",
               Debug::MessageSeverity::eInformation);
}

void VulkanRenderer::createGraphicsPipeline() {
    auto shaderCode = readFile("shaders/shader.spv");
    vk::raii::ShaderModule shaderModule = createShaderModule(shaderCode);

    vk::PipelineShaderStageCreateInfo vertexShaderStageInfo =
        vk::PipelineShaderStageCreateInfo()
            .setStage(vk::ShaderStageFlagBits::eVertex)
            .setModule(shaderModule)
            .setPName("vertMain");

    vk::PipelineShaderStageCreateInfo fragmentShaderStageInfo =
        vk::PipelineShaderStageCreateInfo()
            .setStage(vk::ShaderStageFlagBits::eFragment)
            .setModule(shaderModule)
            .setPName("fragMain");

    vk::PipelineShaderStageCreateInfo shaderStages[] = {
        vertexShaderStageInfo, fragmentShaderStageInfo};

    auto bindingDescription = Vertex::getBindingDescription();
    auto attributeDescriptions = Vertex::getAttributeDescriptions();
    vk::PipelineVertexInputStateCreateInfo vertexInputInfo =
        vk::PipelineVertexInputStateCreateInfo()
            .setVertexBindingDescriptionCount(1)
            .setPVertexBindingDescriptions(&bindingDescription)
            .setVertexAttributeDescriptionCount(
                static_cast<uint32_t>(attributeDescriptions.size()))
            .setPVertexAttributeDescriptions(attributeDescriptions.data());

    vk::PipelineInputAssemblyStateCreateInfo inputAssembly =
        vk::PipelineInputAssemblyStateCreateInfo().setTopology(
            vk::PrimitiveTopology::eTriangleList);

    vk::PipelineViewportStateCreateInfo viewportState =
        vk::PipelineViewportStateCreateInfo()
            .setViewportCount(1)
            .setScissorCount(1);

    vk::PipelineRasterizationStateCreateInfo rasterizer =
        vk::PipelineRasterizationStateCreateInfo()
            .setDepthClampEnable(vk::False)
            .setRasterizerDiscardEnable(vk::False)
            .setPolygonMode(vk::PolygonMode::eFill)
            .setCullMode(vk::CullModeFlagBits::eBack)
            .setFrontFace(vk::FrontFace::eCounterClockwise)
            .setDepthBiasEnable(vk::False)
            .setDepthBiasSlopeFactor(1.0f)
            .setLineWidth(1.0f);

    vk::PipelineMultisampleStateCreateInfo multisampling =
        vk::PipelineMultisampleStateCreateInfo()
            .setRasterizationSamples(vk::SampleCountFlagBits::e1)
            .setSampleShadingEnable(vk::False);

    vk::PipelineColorBlendAttachmentState colorBlendAttachment =
        vk::PipelineColorBlendAttachmentState()
            .setColorWriteMask(vk::ColorComponentFlagBits::eR |
                               vk::ColorComponentFlagBits::eG |
                               vk::ColorComponentFlagBits::eB |
                               vk::ColorComponentFlagBits::eA)
            .setBlendEnable(vk::False);

    vk::PipelineColorBlendStateCreateInfo colorBlending =
        vk::PipelineColorBlendStateCreateInfo()
            .setLogicOpEnable(vk::False)
            .setLogicOp(vk::LogicOp::eCopy)
            .setAttachmentCount(1)
            .setPAttachments(&colorBlendAttachment);

    std::vector<vk::DynamicState> dynamicStates = {vk::DynamicState::eViewport,
                                                   vk::DynamicState::eScissor};
    vk::PipelineDynamicStateCreateInfo dynamicStateInfo(
        {}, static_cast<uint32_t>(dynamicStates.size()), dynamicStates.data());

    vk::PipelineLayoutCreateInfo pipelineLayoutInfo =
        vk::PipelineLayoutCreateInfo()
            .setSetLayoutCount(1)
            .setPSetLayouts(&*m_descriptorSetLayout)
            .setPushConstantRangeCount(0);

    m_pipelineLayout =
        vk::raii::PipelineLayout(m_logicalDevice, pipelineLayoutInfo);

    Debug::log("[Vulkan] Created: Pipeline Layout (Graphics)",
               Debug::MessageSeverity::eInformation);

    vk::GraphicsPipelineCreateInfo graphicsPipelineInfo =
        vk::GraphicsPipelineCreateInfo()
            .setStageCount(2)
            .setStages(shaderStages)
            .setPVertexInputState(&vertexInputInfo)
            .setPInputAssemblyState(&inputAssembly)
            .setPViewportState(&viewportState)
            .setPRasterizationState(&rasterizer)
            .setPMultisampleState(&multisampling)
            .setPColorBlendState(&colorBlending)
            .setPDynamicState(&dynamicStateInfo)
            .setLayout(m_pipelineLayout)
            .setRenderPass(nullptr);

    vk::PipelineRenderingCreateInfo pipelineRenderingInfo =
        vk::PipelineRenderingCreateInfo()
            .setColorAttachmentCount(1)
            .setPColorAttachmentFormats(&m_swapchainSurfaceFormat.format);

    vk::StructureChain<vk::GraphicsPipelineCreateInfo,
                       vk::PipelineRenderingCreateInfo>
        pipelineCreateInfoChain(graphicsPipelineInfo, pipelineRenderingInfo);

    m_graphicsPipeline = vk::raii::Pipeline(
        m_logicalDevice, nullptr,
        pipelineCreateInfoChain.get<vk::GraphicsPipelineCreateInfo>());

    Debug::log("[Vulkan] Created: Pipeline (Graphics)",
               Debug::MessageSeverity::eInformation);
}

void VulkanRenderer::createCommandPool() {
    vk::CommandPoolCreateInfo poolInfo =
        vk::CommandPoolCreateInfo()
            .setFlags(vk::CommandPoolCreateFlagBits::eResetCommandBuffer)
            .setQueueFamilyIndex(m_queueIndex);

    m_commandPool = vk::raii::CommandPool(m_logicalDevice, poolInfo);

    Debug::log("[Vulkan] Created: Command Pool",
               Debug::MessageSeverity::eInformation);
}

void VulkanRenderer::createTextureImage() {
    int textureWidth;
    int textureHeight;
    int textureChannels;

    stbi_uc *pixels =
        stbi_load("textures/parrot.jpg", &textureWidth, &textureHeight,
                  &textureChannels, STBI_rgb_alpha);

    const vk::DeviceSize imageSize = textureWidth * textureHeight * 4;
    if (!pixels) {
        throw std::runtime_error(
            "Error: Failed to load parrot texture image!\n");
    }

    vk::raii::Buffer stagingBuffer({});
    vk::raii::DeviceMemory stagingBufferMemory({});

    createBuffer(imageSize, vk::BufferUsageFlagBits::eTransferSrc,
                 vk::MemoryPropertyFlagBits::eHostVisible |
                     vk::MemoryPropertyFlagBits::eHostCoherent,
                 stagingBuffer, stagingBufferMemory);

    void *data = stagingBufferMemory.mapMemory(0, imageSize);
    memcpy(data, pixels, imageSize);
    stagingBufferMemory.unmapMemory();

    stbi_image_free(pixels);

    createImage(
        textureWidth, textureHeight, vk::Format::eR8G8B8A8Srgb,
        vk::ImageTiling::eOptimal,
        vk::ImageUsageFlagBits::eTransferDst | vk::ImageUsageFlagBits::eSampled,
        vk::MemoryPropertyFlagBits::eDeviceLocal, m_textureImage,
        m_textureImageMemory);

    transitionImageLayout(m_textureImage, vk::ImageLayout::eUndefined,
                          vk::ImageLayout::eTransferDstOptimal);
    copyBufferToImage(stagingBuffer, m_textureImage,
                      static_cast<uint32_t>(textureWidth),
                      static_cast<uint32_t>(textureHeight));
    transitionImageLayout(m_textureImage, vk::ImageLayout::eTransferDstOptimal,
                          vk::ImageLayout::eShaderReadOnlyOptimal);

    Debug::log("[Vulkan] Created: Texture Image",
               Debug::MessageSeverity::eInformation);
}

void VulkanRenderer::createTextureImageView() {
    m_textureImageView =
        createImageView(m_textureImage, vk::Format::eR8G8B8A8Srgb);

    Debug::log("[Vulkan] Created: Texture ImageView",
               Debug::MessageSeverity::eInformation);
}

void VulkanRenderer::createTextureSampler() {
    vk::PhysicalDeviceProperties properties = m_physicalDevice.getProperties();

    vk::SamplerCreateInfo samplerInfo =
        vk::SamplerCreateInfo()
            .setMagFilter(vk::Filter::eLinear)
            .setMinFilter(vk::Filter::eLinear)
            .setMipmapMode(vk::SamplerMipmapMode::eLinear)
            .setAddressModeU(vk::SamplerAddressMode::eRepeat)
            .setAddressModeV(vk::SamplerAddressMode::eRepeat)
            .setAddressModeW(vk::SamplerAddressMode::eRepeat)
            .setMipLodBias(0.0f)
            .setAnisotropyEnable(vk::True)
            .setMaxAnisotropy(properties.limits.maxSamplerAnisotropy)
            .setCompareEnable(vk::False)
            .setCompareOp(vk::CompareOp::eAlways);

    m_textureSampler = vk::raii::Sampler(m_logicalDevice, samplerInfo);

    Debug::log("[Vulkan] Created: Texture Sampler",
               Debug::MessageSeverity::eInformation);
}

void VulkanRenderer::createVertexBuffer() {
    // Create temporary host-visible staging buffer
    vk::DeviceSize bufferSize = sizeof(m_vertices[0]) * m_vertices.size();
    vk::raii::Buffer stagingBuffer({});
    vk::raii::DeviceMemory stagingBufferMemory({});

    createBuffer(bufferSize, vk::BufferUsageFlagBits::eTransferSrc,
                 vk::MemoryPropertyFlagBits::eHostVisible |
                     vk::MemoryPropertyFlagBits::eHostCoherent,
                 stagingBuffer, stagingBufferMemory);

    void *stagingData = stagingBufferMemory.mapMemory(0, bufferSize);
    memcpy(stagingData, m_vertices.data(), bufferSize);
    stagingBufferMemory.unmapMemory();

    createBuffer(bufferSize,
                 vk::BufferUsageFlagBits::eTransferDst |
                     vk::BufferUsageFlagBits::eVertexBuffer,
                 vk::MemoryPropertyFlagBits::eDeviceLocal, m_vertexBuffer,
                 m_vertexBufferMemory);

    copyBuffer(stagingBuffer, m_vertexBuffer, bufferSize);
}

void VulkanRenderer::createIndexBuffer() {
    vk::DeviceSize bufferSize = sizeof(m_indices[0]) * m_indices.size();

    vk::raii::Buffer stagingBuffer({});
    vk::raii::DeviceMemory stagingBufferMemory({});
    createBuffer(bufferSize, vk::BufferUsageFlagBits::eTransferSrc,
                 vk::MemoryPropertyFlagBits::eHostVisible |
                     vk::MemoryPropertyFlagBits::eHostCoherent,
                 stagingBuffer, stagingBufferMemory);

    void *data = stagingBufferMemory.mapMemory(0, bufferSize);
    memcpy(data, m_indices.data(), static_cast<size_t>(bufferSize));
    stagingBufferMemory.unmapMemory();

    createBuffer(bufferSize,
                 vk::BufferUsageFlagBits::eTransferDst |
                     vk::BufferUsageFlagBits::eIndexBuffer,
                 vk::MemoryPropertyFlagBits::eDeviceLocal, m_indexBuffer,
                 m_indexBufferMemory);

    copyBuffer(stagingBuffer, m_indexBuffer, bufferSize);
}

void VulkanRenderer::createUniformBuffers() {
    m_uniformBuffers.clear();
    m_uniformBuffersMemory.clear();
    m_uniformBuffersMapped.clear();

    for (size_t i = 0; i < m_kFramesInFlight; ++i) {
        vk::DeviceSize bufferSize = sizeof(UniformBufferObject);
        vk::raii::Buffer buffer({});
        vk::raii::DeviceMemory bufferMemory({});

        createBuffer(bufferSize, vk::BufferUsageFlagBits::eUniformBuffer,
                     vk::MemoryPropertyFlagBits::eHostVisible |
                         vk::MemoryPropertyFlagBits::eHostCoherent,
                     buffer, bufferMemory);

        m_uniformBuffers.emplace_back(std::move(buffer));
        m_uniformBuffersMemory.emplace_back(std::move(bufferMemory));
        m_uniformBuffersMapped.emplace_back(
            m_uniformBuffersMemory[i].mapMemory(0, bufferSize));
    }
}

void VulkanRenderer::createDescriptorPool() {
    std::array<vk::DescriptorPoolSize, 2> poolSize = {
        vk::DescriptorPoolSize(vk::DescriptorType::eUniformBuffer,
                               m_kFramesInFlight),
        vk::DescriptorPoolSize(vk::DescriptorType::eCombinedImageSampler,
                               m_kFramesInFlight)};

    vk::DescriptorPoolCreateInfo poolInfo =
        vk::DescriptorPoolCreateInfo()
            .setFlags(vk::DescriptorPoolCreateFlagBits::eFreeDescriptorSet)
            .setMaxSets(m_kFramesInFlight)
            .setPoolSizeCount(poolSize.size())
            .setPPoolSizes(poolSize.data());

    m_descriptorPool = vk::raii::DescriptorPool(m_logicalDevice, poolInfo);

    Debug::log("[Vulkan] Created: DescriptorPool",
               Debug::MessageSeverity::eInformation);
}

void VulkanRenderer::createDescriptorSets() {
    std::vector<vk::DescriptorSetLayout> layouts(m_kFramesInFlight,
                                                 *m_descriptorSetLayout);
    vk::DescriptorSetAllocateInfo allocInfo =
        vk::DescriptorSetAllocateInfo()
            .setDescriptorPool(m_descriptorPool)
            .setDescriptorSetCount(static_cast<uint32_t>(layouts.size()))
            .setPSetLayouts(layouts.data());

    m_descriptorSets.clear();
    m_descriptorSets = m_logicalDevice.allocateDescriptorSets(allocInfo);

    for (size_t i = 0; i < m_kFramesInFlight; ++i) {
        vk::DescriptorBufferInfo bufferInfo =
            vk::DescriptorBufferInfo()
                .setBuffer(m_uniformBuffers[i])
                .setOffset(0)
                .setRange(sizeof(UniformBufferObject));

        vk::DescriptorImageInfo imageInfo =
            vk::DescriptorImageInfo()
                .setSampler(m_textureSampler)
                .setImageView(m_textureImageView)
                .setImageLayout(vk::ImageLayout::eShaderReadOnlyOptimal);

        std::array<vk::WriteDescriptorSet, 2> descriptorWrites{
            vk::WriteDescriptorSet()
                .setDstSet(m_descriptorSets[i])
                .setDstBinding(0)
                .setDstArrayElement(0)
                .setDescriptorCount(1)
                .setDescriptorType(vk::DescriptorType::eUniformBuffer)
                .setPBufferInfo(&bufferInfo),

            vk::WriteDescriptorSet()
                .setDstSet(m_descriptorSets[i])
                .setDstBinding(1)
                .setDstArrayElement(0)
                .setDescriptorCount(1)
                .setDescriptorType(vk::DescriptorType::eCombinedImageSampler)
                .setPImageInfo(&imageInfo)};

        m_logicalDevice.updateDescriptorSets(descriptorWrites, {});
    }

    Debug::log("[Vulkan] Created: DescriptorSets",
               Debug::MessageSeverity::eInformation);
}

void VulkanRenderer::createCommandBuffers() {
    vk::CommandBufferAllocateInfo allocInfo =
        vk::CommandBufferAllocateInfo()
            .setCommandPool(m_commandPool)
            .setLevel(vk::CommandBufferLevel::ePrimary)
            .setCommandBufferCount(m_kFramesInFlight);

    vk::raii::CommandBuffers commandBuffers(m_logicalDevice, allocInfo);

    m_commandBuffers.clear();
    m_commandBuffers.reserve(m_kFramesInFlight);
    for (uint32_t i = 0; i < m_kFramesInFlight; ++i) {
        m_commandBuffers.emplace_back(std::move(commandBuffers[i]));
    }

    Debug::log("[Vulkan] Created: CommandBuffers",
               Debug::MessageSeverity::eInformation);
}

void VulkanRenderer::createSyncObjects() {
    m_presentCompleteSemaphores.clear();
    m_renderFinishedSemaphores.clear();
    m_inFlightFences.clear();

    for (uint32_t i = 0; i < m_swapchainImages.size(); ++i) {
        m_presentCompleteSemaphores.emplace_back(m_logicalDevice,
                                                 vk::SemaphoreCreateInfo());
        m_renderFinishedSemaphores.emplace_back(m_logicalDevice,
                                                vk::SemaphoreCreateInfo());
    }

    for (uint32_t i = 0; i < m_kFramesInFlight; ++i) {
        m_inFlightFences.emplace_back(
            m_logicalDevice,
            vk::FenceCreateInfo(vk::FenceCreateFlagBits::eSignaled), nullptr);
    }

    Debug::log("[Vulkan] Created: Sync Objects",
               Debug::MessageSeverity::eInformation);
}

}  // namespace avenir::graphics::vulkan