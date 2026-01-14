#pragma once

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/hash.hpp>

#include <iostream>
#include <fstream>
#include <string>
#include <stdexcept>
#include <algorithm>
#include <chrono>
#include <vector>
#include <cstring>
#include <cstdlib>
#include <cstdint>
#include <limits>
#include <array>
#include <optional>
#include <set>
#include <unordered_map>
#include <memory>

// Forward declarations or include VulkanRenderer for shared structs
#include "VulkanRenderer.h"

const std::vector<const char *> validationLayers = {
    "VK_LAYER_KHRONOS_validation"};

const std::vector<const char *> deviceExtensions = {
    VK_KHR_SWAPCHAIN_EXTENSION_NAME,
    VK_KHR_DYNAMIC_RENDERING_EXTENSION_NAME};

#ifdef NDEBUG
const bool enableValidationLayers = false;
#else
const bool enableValidationLayers = true;
#endif

VkResult CreateDebugUtilsMessengerEXT(VkInstance instance, const VkDebugUtilsMessengerCreateInfoEXT *pCreateInfo, const VkAllocationCallbacks *pAllocator, VkDebugUtilsMessengerEXT *pDebugMessenger);

void DestroyDebugUtilsMessengerEXT(VkInstance instance, VkDebugUtilsMessengerEXT debugMessenger, const VkAllocationCallbacks *pAllocator);

class VulkanInitializer
{
public:
    VulkanInitializer();
    ~VulkanInitializer();

    void initialize(GLFWwindow* window);
    void cleanup();

    // Getters for Vulkan objects
    VkInstance getInstance() const { return instance; }
    VkPhysicalDevice getPhysicalDevice() const { return physicalDevice; }
    VkDevice getDevice() const { return device; }
    VkSurfaceKHR getSurface() const { return surface; }
    VkQueue getGraphicsQueue() const { return graphicsQueue; }
    VkQueue getPresentQueue() const { return presentQueue; }
    VkSampleCountFlagBits getMsaaSamples() const { return msaaSamples; }

private:
    VkInstance instance;
    VkDebugUtilsMessengerEXT debugMessenger;
    VkSurfaceKHR surface;

    VkPhysicalDevice physicalDevice = VK_NULL_HANDLE;
    VkSampleCountFlagBits msaaSamples = VK_SAMPLE_COUNT_64_BIT;
    VkDevice device;

    VkQueue graphicsQueue;
    VkQueue presentQueue;

    bool isInitialized;

    void createInstance();
    void populateDebugMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT &createInfo);
    void setupDebugMessenger();
    void createSurface(GLFWwindow* window);
    void pickPhysicalDevice();
    void createLogicalDevice();

    // Helper methods
    SwapChainSupportDetails querySwapChainSupport(VkPhysicalDevice device);
    bool isDeviceSuitable(VkPhysicalDevice device);
    bool checkDeviceExtensionSupport(VkPhysicalDevice device);
    QueueFamilyIndices findQueueFamilies(VkPhysicalDevice device);
    VkSampleCountFlagBits getMaxUsableSampleCount();
    std::vector<const char *> getRequiredExtensions();
    bool checkValidationLayerSupport();
    static std::vector<char> readFile(const std::string &filename);
    static VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback(VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity, VkDebugUtilsMessageTypeFlagsEXT messageType, const VkDebugUtilsMessengerCallbackDataEXT *pCallbackData, void *pUserData);
};
