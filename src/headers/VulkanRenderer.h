#pragma once

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <vector>
#include <memory>
#include <array>

#include "Model.h"
#include "SceneManager.h"
#include "ShaderParameter.h"

// Forward declarations
class VulkanSwapChain;
class VulkanTexture;
class VulkanBuffer;
class VulkanPipeline;
class VulkanCommandManager;
class VulkanDescriptorManager;

const int MAX_FRAMES_IN_FLIGHT = 2;

// Shared structures
struct UniformBufferObject
{
    alignas(16) glm::mat4 view;
    alignas(16) glm::mat4 proj;
    alignas(16) glm::vec3 cameraPos;
    alignas(16) glm::vec3 lightDir;     // direction TO light (normalized)
    alignas(16) glm::vec3 lightColor;   // light radiance
    alignas(16) glm::vec4 fogColor;     // fog color (scene-wide)
    alignas(4) float fogDensity;        // fog density factor
    alignas(4) float fogStart;          // minimum distance before fog starts
};

struct MaterialUniformBufferObject
{
    alignas(16) glm::vec4 baseColorFactor;    // base color multiplier
    alignas(16) glm::vec3 emissiveFactor;     // emissive multiplier
    alignas(4) float metallic;                // metallic factor (0.0 = dielectric, 1.0 = metal)
    alignas(4) float roughness;               // roughness factor (0.0 = smooth, 1.0 = rough)
};

// Indirect rendering structures
struct IndirectInstanceData
{
    glm::mat4 modelMatrix;           // Model transformation matrix
    glm::vec4 baseColorFactor;       // Material base color
    glm::vec3 emissiveFactor;        // Material emissive factor
    float metallic;                  // Material metallic value
    float roughness;                 // Material roughness value
    uint32_t baseColorTextureIndex;  // Index into texture array (0 = default white)
    uint32_t metallicRoughnessTextureIndex; // Index into texture array (0 = default)
    uint32_t emissiveTextureIndex;   // Index into texture array (0 = default)
    uint32_t vertexBufferIndex;      // Index of vertex buffer to use
    uint32_t indexBufferIndex;       // Index of index buffer to use
    uint32_t firstIndex;             // First index in index buffer
    uint32_t indexCount;             // Number of indices to draw
    uint32_t vertexOffset;           // Vertex offset in vertex buffer
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
    
    // Getters
    bool wasFramebufferResized() const { return framebufferResized; }
    void setFramebufferResized(bool resized) { framebufferResized = resized; }
    
    // Texture management
    void loadTexture(const std::string& texturePath);
    void loadRequiredTextures(std::shared_ptr<SceneManager> sceneManager);

    // Helper functions for Model class
    VkCommandPool getCommandPool() const;
    void createBuffer(VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties, 
                     VkBuffer& buffer, VkDeviceMemory& bufferMemory);
    VkCommandBuffer beginSingleTimeCommands();
    void endSingleTimeCommands(VkCommandBuffer commandBuffer);
    void copyBuffer(VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize size);
    uint32_t findMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties);

private:
    // Vulkan handles (not owned by renderer)
    VkPhysicalDevice physicalDevice;
    VkDevice device;
    VkQueue graphicsQueue;
    VkQueue presentQueue;
    VkSurfaceKHR surface;
    GLFWwindow* window;
    VkSampleCountFlagBits msaaSamples;
    
    // Helper classes
    std::shared_ptr<VulkanSwapChain> swapChain;
    std::shared_ptr<VulkanTexture> textureManager;
    std::shared_ptr<VulkanBuffer> bufferManager;
    std::shared_ptr<VulkanPipeline> pipelineManager;
    std::shared_ptr<VulkanCommandManager> commandManager;
    std::shared_ptr<VulkanDescriptorManager> descriptorManager;

    // MSAA resources
    VkImage colorImage = VK_NULL_HANDLE;
    VkDeviceMemory colorImageMemory = VK_NULL_HANDLE;
    VkImageView colorImageView = VK_NULL_HANDLE;
    
    // Depth resources
    VkImage depthImage = VK_NULL_HANDLE;
    VkDeviceMemory depthImageMemory = VK_NULL_HANDLE;
    VkImageView depthImageView = VK_NULL_HANDLE;
    
    // Uniform buffers
    std::vector<VkBuffer> uniformBuffers;
    std::vector<VkDeviceMemory> uniformBuffersMemory;
    std::vector<void*> uniformBuffersMapped;

    // Material uniform buffers
    std::vector<VkBuffer> materialUniformBuffers;
    std::vector<VkDeviceMemory> materialUniformBuffersMemory;
    std::vector<void*> materialUniformBuffersMapped;
    
    // Indirect rendering resources
    std::vector<VkBuffer> indirectCommandBuffers;
    std::vector<VkDeviceMemory> indirectCommandBuffersMemory;
    std::vector<void*> indirectCommandBuffersMapped;
    std::vector<VkBuffer> indirectInstanceBuffers;
    std::vector<VkDeviceMemory> indirectInstanceBuffersMemory;
    std::vector<void*> indirectInstanceBuffersMapped;
    std::vector<uint32_t> indirectDrawCounts;

    // Synchronization
    std::vector<VkSemaphore> imageAvailableSemaphores;
    std::vector<VkSemaphore> renderFinishedSemaphores;
    std::vector<VkFence> inFlightFences;
    uint32_t currentFrame = 0;

    bool framebufferResized = false;

    // FPS tracking
    double lastTime = 0.0;
    int frameCount = 0;
    double fps = 0.0;
    double startTime = 0.0;
    
    // Internal methods
    void createColorResources();
    void createDepthResources();
    void cleanupSwapChainResources();
    void createUniformBuffers();
    void createMaterialUniformBuffers();
    void createIndirectBuffers();
    void createSyncObjects();
    
    void recordCommandBuffer(VkCommandBuffer commandBuffer, uint32_t imageIndex,
                           std::shared_ptr<SceneManager> sceneManager);
    void updateUniformBuffer(uint32_t currentImage, std::shared_ptr<SceneManager> sceneManager);
    
    VkFormat findDepthFormat();
    VkFormat findSupportedFormat(const std::vector<VkFormat>& candidates, VkImageTiling tiling, VkFormatFeatureFlags features);
};
