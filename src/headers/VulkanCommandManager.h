#pragma once

#include <vulkan/vulkan.h>
#include <vector>

class VulkanCommandManager
{
public:
    VulkanCommandManager(VkPhysicalDevice physicalDevice, VkDevice device, VkQueue graphicsQueue, VkSurfaceKHR surface);
    ~VulkanCommandManager();

    void createCommandPool();
    void createCommandBuffers(uint32_t count);
    
    VkCommandBuffer beginSingleTimeCommands();
    void endSingleTimeCommands(VkCommandBuffer commandBuffer);

    VkCommandPool getCommandPool() const { return commandPool; }
    const std::vector<VkCommandBuffer>& getCommandBuffers() const { return commandBuffers; }
    VkCommandBuffer getCommandBuffer(uint32_t index) const { return commandBuffers[index]; }

    void cleanup();

private:
    VkPhysicalDevice physicalDevice;
    VkDevice device;
    VkQueue graphicsQueue;
    VkSurfaceKHR surface;

    VkCommandPool commandPool = VK_NULL_HANDLE;
    std::vector<VkCommandBuffer> commandBuffers;
};
