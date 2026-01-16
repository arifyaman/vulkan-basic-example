#pragma once

#include <vulkan/vulkan.h>
#include <vector>
#include <memory>

class VulkanBuffer;

class VulkanDescriptorManager
{
public:
    VulkanDescriptorManager(VkDevice device, uint32_t maxFramesInFlight);
    ~VulkanDescriptorManager();

    void createDescriptorSetLayout();
    void createDescriptorPool();
    void createDescriptorSets(const std::vector<VkBuffer>& uniformBuffers,
                             const std::vector<VkBuffer>& materialUniformBuffers,
                             const std::vector<VkBuffer>& indirectInstanceBuffers,
                             VkImageView defaultTextureView,
                             VkSampler textureSampler);

    VkDescriptorSetLayout getDescriptorSetLayout() const { return descriptorSetLayout; }
    VkDescriptorPool getDescriptorPool() const { return descriptorPool; }
    const std::vector<VkDescriptorSet>& getDescriptorSets() const { return descriptorSets; }
    VkDescriptorSet getDescriptorSet(uint32_t index) const { return descriptorSets[index]; }

    void cleanup();

private:
    VkDevice device;
    uint32_t maxFramesInFlight;

    VkDescriptorSetLayout descriptorSetLayout = VK_NULL_HANDLE;
    VkDescriptorPool descriptorPool = VK_NULL_HANDLE;
    std::vector<VkDescriptorSet> descriptorSets;
};
