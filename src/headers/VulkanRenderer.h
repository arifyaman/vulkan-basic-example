#pragma once

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <vector>
#include <memory>
#include <array>
#include <optional>
#include <unordered_map>

#include "Model.h"
#include "SceneManager.h"
#include "ShaderParameter.h"

const int MAX_FRAMES_IN_FLIGHT = 2;

// Forward declarations and shared structures
struct QueueFamilyIndices;
struct SwapChainSupportDetails;
struct UniformBufferObject;

// Texture resource structure
struct TextureResource {
    VkImage image;
    VkDeviceMemory memory;
    VkImageView view;
    uint32_t mipLevels;
};

// Shared structures
struct QueueFamilyIndices
{
    std::optional<uint32_t> graphicsFamily;
    std::optional<uint32_t> presentFamily;

    bool isComplete()
    {
        return graphicsFamily.has_value() && presentFamily.has_value();
    }
};

struct SwapChainSupportDetails
{
    VkSurfaceCapabilitiesKHR capabilities;
    std::vector<VkSurfaceFormatKHR> formats;
    std::vector<VkPresentModeKHR> presentModes;
};

struct UniformBufferObject
{
    alignas(16) glm::mat4 view;
    alignas(16) glm::mat4 proj;
    alignas(16) glm::vec3 cameraPos;
    alignas(16) glm::vec3 lightDir;     // direction TO light (normalized)
    alignas(16) glm::vec3 lightColor;   // light radiance
    alignas(4) float fogDensity;        // fog density factor
};

struct MaterialUniformBufferObject
{
    alignas(16) glm::vec4 baseColorFactor;    // base color multiplier
    alignas(16) glm::vec3 emissiveFactor;     // emissive multiplier
    alignas(16) glm::vec4 fogColor;           // fog color
    alignas(4) float metallic;                // metallic factor (0.0 = dielectric, 1.0 = metal)
    alignas(4) float roughness;               // roughness factor (0.0 = smooth, 1.0 = rough)
};

class VulkanRenderer
{
public:
    VulkanRenderer(VkPhysicalDevice physicalDevice, VkDevice device, VkQueue graphicsQueue, 
                   VkQueue presentQueue, VkSurfaceKHR surface, GLFWwindow* window, 
                   VkSampleCountFlagBits msaaSamples);
    ~VulkanRenderer();

    // Initialization
    void initialize();
    void cleanup();
    
    // Shader loading (call after initialize, before first frame)
    void createGraphicsPipelines(const std::vector<std::string>& shaderNames);
    
    // Frame rendering
    void drawFrame(std::shared_ptr<SceneManager> sceneManager);
    
    // Swap chain management
    void recreateSwapChain();
    void cleanupSwapChain();
    
    // Getters
    VkCommandPool getCommandPool() const { return commandPool; }
    bool wasFramebufferResized() const { return framebufferResized; }
    void setFramebufferResized(bool resized) { framebufferResized = resized; }
    
    // Resource creation helpers (for use by Model class and others)
    void createBuffer(VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties, 
                     VkBuffer& buffer, VkDeviceMemory& bufferMemory);
    VkCommandBuffer beginSingleTimeCommands();
    void endSingleTimeCommands(VkCommandBuffer commandBuffer);
    void copyBuffer(VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize size);
    uint32_t findMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties);
    
    VkImageView createImageView(VkImage image, VkFormat format, VkImageAspectFlags aspectFlags, uint32_t mipLevels);
    void createImage(uint32_t width, uint32_t height, uint32_t mipLevels, VkSampleCountFlagBits numSamples,
                    VkFormat format, VkImageTiling tiling, VkImageUsageFlags usage, 
                    VkMemoryPropertyFlags properties, VkImage& image, VkDeviceMemory& imageMemory);
    void transitionImageLayout(VkImage image, VkFormat format, VkImageLayout oldLayout,
                             VkImageLayout newLayout, uint32_t mipLevels);
    void copyBufferToImage(VkBuffer buffer, VkImage image, uint32_t width, uint32_t height);
    void generateMipmaps(VkImage image, VkFormat imageFormat, int32_t texWidth, int32_t texHeight, uint32_t mipLevels);

    // Texture management
    void loadTexture(const std::string& texturePath);
    void loadRequiredTextures(std::shared_ptr<SceneManager> sceneManager);

private:
    // Vulkan handles (not owned by renderer)
    VkPhysicalDevice physicalDevice;
    VkDevice device;
    VkQueue graphicsQueue;
    VkQueue presentQueue;
    VkSurfaceKHR surface;
    GLFWwindow* window;
    VkSampleCountFlagBits msaaSamples;
    
    // Swap chain
    VkSwapchainKHR swapChain;
    std::vector<VkImage> swapChainImages;
    VkFormat swapChainImageFormat;
    VkExtent2D swapChainExtent;
    std::vector<VkImageView> swapChainImageViews;
    std::vector<VkFramebuffer> swapChainFramebuffers;
    
    // Pipeline
    VkRenderPass renderPass;
    VkDescriptorSetLayout descriptorSetLayout;
    VkPipelineLayout pipelineLayout;
    std::unordered_map<std::string, VkPipeline> graphicsPipelines; // Multiple pipelines by shader name
    
    // Command buffers
    VkCommandPool commandPool;
    std::vector<VkCommandBuffer> commandBuffers;
    
    // MSAA
    VkImage colorImage;
    VkDeviceMemory colorImageMemory;
    VkImageView colorImageView;
    
    // Depth
    VkImage depthImage;
    VkDeviceMemory depthImageMemory;
    VkImageView depthImageView;
    
    // Texture
    uint32_t mipLevels;
    VkImage textureImage;
    VkDeviceMemory textureImageMemory;
    VkImageView textureImageView;
    VkSampler textureSampler;

    // Default white texture for materials without textures
    VkImage defaultTextureImage;
    VkDeviceMemory defaultTextureImageMemory;
    VkImageView defaultTextureImageView;


    
    // Uniform buffers
    std::vector<VkBuffer> uniformBuffers;
    std::vector<VkDeviceMemory> uniformBuffersMemory;
    std::vector<void*> uniformBuffersMapped;

    // Material uniform buffers (for PBR material properties)
    std::vector<VkBuffer> materialUniformBuffers;
    std::vector<VkDeviceMemory> materialUniformBuffersMemory;
    std::vector<void*> materialUniformBuffersMapped;
    
    // Descriptors
    VkDescriptorPool descriptorPool;
    std::vector<VkDescriptorSet> descriptorSets;
    
    // Synchronization
    std::vector<VkSemaphore> imageAvailableSemaphores;
    std::vector<VkSemaphore> renderFinishedSemaphores;
    std::vector<VkFence> inFlightFences;
    uint32_t currentFrame = 0;

    bool framebufferResized = false;

    // Texture cache for dynamic texture loading
    std::unordered_map<std::string, TextureResource> textureCache;
    
    // FPS tracking
    double lastTime = 0.0;
    int frameCount = 0;
    double fps = 0.0;
    double startTime = 0.0; // Application start time for animations
    
    // Internal methods
    void createSwapChain();
    void createImageViews();
    void createRenderPass();
    void createDescriptorSetLayout();
    void createGraphicsPipeline();
    void createGraphicsPipeline(const std::string& shaderName); // Create pipeline for specific shader
    void createFramebuffers();
    void createCommandPool();
    void createColorResources();
    void createDepthResources();
    void createTextureImage();
    void createTextureImageView();
    void createTextureSampler();
    void createDefaultTexture();
    void createUniformBuffers();
    void createMaterialUniformBuffers();
    void createDescriptorPool();
    void createDescriptorSets();
    void createCommandBuffers();
    void createSyncObjects();
    
    void recordCommandBuffer(VkCommandBuffer commandBuffer, uint32_t imageIndex,
                           std::shared_ptr<SceneManager> sceneManager);
    void updateUniformBuffer(uint32_t currentImage, std::shared_ptr<SceneManager> sceneManager);
    
    VkShaderModule createShaderModule(const std::vector<char>& code);
    VkSurfaceFormatKHR chooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& availableFormats);
    VkPresentModeKHR chooseSwapPresentMode(const std::vector<VkPresentModeKHR>& availablePresentModes);
    VkExtent2D chooseSwapExtent(const VkSurfaceCapabilitiesKHR& capabilities);
    
    VkFormat findSupportedFormat(const std::vector<VkFormat>& candidates, VkImageTiling tiling, VkFormatFeatureFlags features);
    VkFormat findDepthFormat();
    bool hasStencilComponent(VkFormat format);
    
    SwapChainSupportDetails querySwapChainSupport(VkPhysicalDevice device);
    QueueFamilyIndices findQueueFamilies(VkPhysicalDevice device);
    
    static std::vector<char> readFile(const std::string& filename);
};
