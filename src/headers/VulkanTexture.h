#pragma once

#include <vulkan/vulkan.h>
#include <string>
#include <unordered_map>
#include <memory>

class VulkanCommandManager;

struct TextureResource
{
    VkImage image;
    VkDeviceMemory memory;
    VkImageView imageView;
    uint32_t mipLevels;
};

class VulkanTexture
{
public:
    VulkanTexture(VkPhysicalDevice physicalDevice, VkDevice device, std::shared_ptr<VulkanCommandManager> commandManager);
    ~VulkanTexture();

    void createTextureSampler();
    void createDefaultTexture();
    void loadTexture(const std::string& texturePath);
    
    VkImageView getDefaultTextureView() const { return defaultTextureImageView; }
    VkSampler getTextureSampler() const { return textureSampler; }
    const TextureResource* getTexture(const std::string& path) const;
    bool hasTexture(const std::string& path) const { return textureCache.find(path) != textureCache.end(); }

    // Image utility functions
    VkImageView createImageView(VkImage image, VkFormat format, VkImageAspectFlags aspectFlags, uint32_t mipLevels);
    void createImage(uint32_t width, uint32_t height, uint32_t mipLevels, VkSampleCountFlagBits numSamples,
                    VkFormat format, VkImageTiling tiling, VkImageUsageFlags usage,
                    VkMemoryPropertyFlags properties, VkImage& image, VkDeviceMemory& imageMemory);
    void transitionImageLayout(VkImage image, VkFormat format, VkImageLayout oldLayout, 
                              VkImageLayout newLayout, uint32_t mipLevels);
    void copyBufferToImage(VkBuffer buffer, VkImage image, uint32_t width, uint32_t height);
    void generateMipmaps(VkImage image, VkFormat imageFormat, int32_t texWidth, int32_t texHeight, uint32_t mipLevels);

    void cleanup();

private:
    VkPhysicalDevice physicalDevice;
    VkDevice device;
    std::shared_ptr<VulkanCommandManager> commandManager;

    VkSampler textureSampler = VK_NULL_HANDLE;
    VkImage defaultTextureImage = VK_NULL_HANDLE;
    VkDeviceMemory defaultTextureImageMemory = VK_NULL_HANDLE;
    VkImageView defaultTextureImageView = VK_NULL_HANDLE;

    std::unordered_map<std::string, TextureResource> textureCache;

    uint32_t findMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties);
    bool hasStencilComponent(VkFormat format);
};
