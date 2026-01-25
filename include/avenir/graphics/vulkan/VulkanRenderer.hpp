#ifndef VULKANRENDERER_HPP
#define VULKANRENDERER_HPP

#include <array>
#include <vector>
#include <filesystem>

#include <vulkan/vulkan_raii.hpp>

// REQUIRED only for GLFW CreateWindowSurface.
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#define GLM_FORCE_RADIANS
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "avenir/graphics/stb_image.h"

#include "avenir/graphics/Renderer.hpp"

namespace avenir::graphics::vulkan {
class VulkanRenderer final : public Renderer {
public:
    explicit VulkanRenderer(GLFWwindow *window);
    ~VulkanRenderer() override;

    void drawFrame(glm::mat4 cameraViewMatrix) override;
    void onFramebufferResize(int width, int height) override;

private:
    struct Vertex {
        glm::vec2 position;
        glm::vec3 color;
        glm::vec2 textureCoordinates;

        static vk::VertexInputBindingDescription getBindingDescription() {
            return {0, sizeof(Vertex), vk::VertexInputRate::eVertex};
        }

        static std::array<vk::VertexInputAttributeDescription, 3>
        getAttributeDescriptions() {
            return {vk::VertexInputAttributeDescription(
                        0, 0, vk::Format::eR32G32Sfloat,
                        offsetof(Vertex, position)),

                    vk::VertexInputAttributeDescription(
                        1, 0, vk::Format::eR32G32B32Sfloat,
                        offsetof(Vertex, color)),

                    vk::VertexInputAttributeDescription(
                        2, 0, vk::Format::eR32G32B32Sfloat,
                        offsetof(Vertex, textureCoordinates))};
        }
    };

    struct UniformBufferObject {
        alignas(16) glm::mat4 model;
        alignas(16) glm::mat4 view;
        alignas(16) glm::mat4 projection;
    };

    void printAllAvailableInstanceExtensions() const;

    [[nodiscard]] std::vector<const char *> findRequiredInstanceLayers() const;

    static std::vector<const char *> findRequiredInstanceExtensions();

    static VKAPI_ATTR vk::Bool32 VKAPI_CALL debugCallback(
        vk::DebugUtilsMessageSeverityFlagBitsEXT severity,
        vk::DebugUtilsMessageTypeFlagsEXT type,
        const vk::DebugUtilsMessengerCallbackDataEXT *pCallbackData, void *);

    static uint32_t chooseSwapMinImageCount(
        vk::SurfaceCapabilitiesKHR const &surfaceCapabilities);

    static vk::SurfaceFormatKHR chooseSwapSurfaceFormat(
        const std::vector<vk::SurfaceFormatKHR> &availableFormats);

    static vk::PresentModeKHR chooseSwapPresentMode(
        const std::vector<vk::PresentModeKHR> &availablePresentModes);

    vk::Extent2D chooseSwapExtent(
        const vk::SurfaceCapabilitiesKHR &capabilities);

    [[nodiscard]] vk::raii::ShaderModule createShaderModule(
        const std::vector<char> &code) const;

    void recordCommandBuffer(uint32_t imageIndex);

    void transitionImageLayout(uint32_t imageIndex, vk::ImageLayout oldLayout,
                               vk::ImageLayout newLayout,
                               vk::AccessFlags2 sourceAccessMask,
                               vk::AccessFlags2 destinationAccessMask,
                               vk::PipelineStageFlags2 sourceStageMask,
                               vk::PipelineStageFlags2 destinationStageMask);

    void transitionImageLayout(const vk::raii::Image &image,
                               vk::ImageLayout oldLayout,
                               vk::ImageLayout newLayout);

    void cleanupSwapchain();

    void recreateSwapchain();

    uint32_t findMemoryType(uint32_t typeFilter,
                            vk::MemoryPropertyFlags properties);

    void createBuffer(vk::DeviceSize size, vk::BufferUsageFlags usage,
                      vk::MemoryPropertyFlags properties,
                      vk::raii::Buffer &buffer,
                      vk::raii::DeviceMemory &bufferMemory);

    void copyBuffer(const vk::raii::Buffer &sourceBuffer,
                    const vk::raii::Buffer &destinationBuffer,
                    vk::DeviceSize size) const;

    void copyBufferToImage(const vk::raii::Buffer &buffer,
                           const vk::raii::Image &image, uint32_t width,
                           uint32_t height) const;

    void updateUniformBuffer(uint32_t currentImage,
                             const glm::mat4 &viewMatrix) const;

    // std::filesystem::path getResourcePath(const std::string& relativePath);
    static std::vector<char> readFile(const std::string &fileName);

    void createImage(uint32_t width, uint32_t height, vk::Format format,
                     vk::ImageTiling tiling, vk::ImageUsageFlags usage,
                     vk::MemoryPropertyFlags properties, vk::raii::Image &image,
                     vk::raii::DeviceMemory &imageMemory);

    [[nodiscard]] vk::raii::CommandBuffer beginSingleTimeCommands() const;

    void endSingleTimeCommands(
        const vk::raii::CommandBuffer &commandBuffer) const;

    [[nodiscard]] vk::raii::ImageView createImageView(vk::raii::Image &image,
                                                      vk::Format format);

    void createInstance();
    void setupDebugMessenger();
    void createSurface();
    void pickPhysicalDevice();
    void createLogicalDevice();
    void createSwapchain();
    void createImageViews();
    void createDescriptorSetLayout();
    void createGraphicsPipeline();
    void createCommandPool();
    void createTextureImage();
    void createTextureImageView();
    void createTextureSampler();
    void createVertexBuffer();
    void createIndexBuffer();
    void createUniformBuffers();
    void createDescriptorPool();
    void createDescriptorSets();
    void createCommandBuffers();
    void createSyncObjects();

    GLFWwindow *m_glfwWindow = nullptr;

    vk::raii::Context m_context;
    vk::raii::Instance m_instance = nullptr;
    vk::raii::DebugUtilsMessengerEXT m_debugMessenger = nullptr;

    vk::raii::SurfaceKHR m_surface = nullptr;
    vk::raii::PhysicalDevice m_physicalDevice = nullptr;
    vk::raii::Device m_logicalDevice = nullptr;

    vk::raii::Queue m_queue = nullptr;
    uint32_t m_queueIndex = ~0;

    vk::raii::SwapchainKHR m_swapchain = nullptr;
    std::vector<vk::Image> m_swapchainImages;
    vk::SurfaceFormatKHR m_swapchainSurfaceFormat;
    vk::Extent2D m_swapchainExtent;
    std::vector<vk::raii::ImageView> m_swapchainImageViews;

    vk::raii::DescriptorSetLayout m_descriptorSetLayout = nullptr;
    vk::raii::PipelineLayout m_pipelineLayout = nullptr;
    vk::raii::Pipeline m_graphicsPipeline = nullptr;
    vk::raii::CommandPool m_commandPool = nullptr;

    vk::raii::Image m_textureImage = nullptr;
    vk::raii::DeviceMemory m_textureImageMemory = nullptr;

    vk::raii::ImageView m_textureImageView = nullptr;
    vk::raii::Sampler m_textureSampler = nullptr;

    vk::raii::Buffer m_vertexBuffer = nullptr;
    vk::raii::DeviceMemory m_vertexBufferMemory = nullptr;
    vk::raii::Buffer m_indexBuffer = nullptr;
    vk::raii::DeviceMemory m_indexBufferMemory = nullptr;
    std::vector<vk::raii::Buffer> m_uniformBuffers;
    std::vector<vk::raii::DeviceMemory> m_uniformBuffersMemory;
    std::vector<void *> m_uniformBuffersMapped;

    vk::raii::DescriptorPool m_descriptorPool = nullptr;
    std::vector<vk::raii::DescriptorSet> m_descriptorSets;

    std::vector<vk::raii::CommandBuffer> m_commandBuffers;
    std::vector<vk::raii::Semaphore> m_presentCompleteSemaphores;
    std::vector<vk::raii::Semaphore> m_renderFinishedSemaphores;
    std::vector<vk::raii::Fence> m_inFlightFences;
    uint32_t m_semaphoreIndex = 0;
    uint32_t m_currentFrame = 0;
    static constexpr uint32_t m_kFramesInFlight = 2;
    bool m_framebufferResized = false;
    bool m_isFirstRun = true;

#ifdef NDEBUG
    static constexpr bool m_shouldUseValidationLayers = false;
#else
    static constexpr bool m_shouldUseValidationLayers = true;
#endif

    const std::vector<const char *> m_validationLayers = {
        "VK_LAYER_KHRONOS_validation"};

    const std::vector<const char *> m_deviceExtensions = {
        vk::KHRSwapchainExtensionName, vk::KHRSpirv14ExtensionName,
        vk::KHRSynchronization2ExtensionName,
        vk::KHRCreateRenderpass2ExtensionName,
#if defined(__APPLE__)
        "VK_KHR_portability_subset"
#endif
    };

    /*
     * Vulkan and glTF models by default handles vertices in a
     * counter-clockwise winding order, as shown below how our quad is being
     * rendered.
     *
     * Vertices:    UV Coordinates:
     * 3  2         0,0  1,0
     * +--+           +--+
     * | /|           | /|
     * |/ |           |/ |
     * +--+           +--+
     * 0  1         0,1  1,1
     */
    const std::vector<Vertex> m_vertices = {
        {{-0.5f, -0.5f}, {1.0f, 0.0f, 0.0f}, {0.0f, 1.0f}},
        {{0.5f, -0.5f}, {0.0f, 1.0f, 0.0f}, {1.0f, 1.0f}},
        {{0.5, 0.5}, {0.0f, 0.0f, 1.0f}, {1.0f, 0.0f}},
        {{-0.5f, 0.5f}, {1.0f, 1.0f, 1.0f}, {0.0f, 0.0f}}};

    const std::vector<uint16_t> m_indices{0, 1, 2, 2, 3, 0};
};
}  // namespace avenir::graphics::vulkan
#endif  // VULKANRENDERER_HPP