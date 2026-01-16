#pragma once

#include <vulkan/vulkan.h>
#include <memory>

class VulkanCommandManager;

class VulkanBuffer
{
public:
    VulkanBuffer(VkPhysicalDevice physicalDevice, VkDevice device, std::shared_ptr<VulkanCommandManager> commandManager);
    ~VulkanBuffer() = default;

    void createBuffer(VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties,
                     VkBuffer& buffer, VkDeviceMemory& bufferMemory);
    void copyBuffer(VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize size);
    uint32_t findMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties);

private:
    VkPhysicalDevice physicalDevice;
    VkDevice device;
    std::shared_ptr<VulkanCommandManager> commandManager;
};
