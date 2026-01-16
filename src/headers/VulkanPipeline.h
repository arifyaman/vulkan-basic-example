#pragma once

#include <vulkan/vulkan.h>
#include <string>
#include <vector>
#include <unordered_map>
#include "Model.h"

class VulkanPipeline
{
public:
    VulkanPipeline(VkPhysicalDevice physicalDevice, VkDevice device, VkSampleCountFlagBits msaaSamples);
    ~VulkanPipeline();

    void createPipelineLayout(VkDescriptorSetLayout descriptorSetLayout);
    void createGraphicsPipeline(const std::string& shaderName, VkFormat swapChainImageFormat, VkExtent2D swapChainExtent);
    void createGraphicsPipelines(const std::vector<std::string>& shaderNames, VkFormat swapChainImageFormat, VkExtent2D swapChainExtent);

    VkPipeline getPipeline(const std::string& name) const;
    VkPipelineLayout getPipelineLayout() const { return pipelineLayout; }
    
    void cleanup();

private:
    VkPhysicalDevice physicalDevice;
    VkDevice device;
    VkSampleCountFlagBits msaaSamples;

    VkPipelineLayout pipelineLayout = VK_NULL_HANDLE;
    std::unordered_map<std::string, VkPipeline> graphicsPipelines;

    VkShaderModule createShaderModule(const std::vector<char>& code);
    std::vector<char> readFile(const std::string& filename);
    VkFormat findDepthFormat();
    VkFormat findSupportedFormat(const std::vector<VkFormat>& candidates, VkImageTiling tiling, VkFormatFeatureFlags features);
};
