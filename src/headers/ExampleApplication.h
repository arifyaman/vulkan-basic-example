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

#include "Model.h"
#include "ModelInstance.h"
#include "SceneManager.h"
#include "VulkanRenderer.h"

const uint32_t WIDTH = 800;
const uint32_t HEIGHT = 600;

const std::string MODEL_PATH = "models/viking_room.obj";
const std::string TEXTURE_PATH = "textures/viking_room.png";

const std::vector<const char *> validationLayers = {
    "VK_LAYER_KHRONOS_validation"};

const std::vector<const char *> deviceExtensions = {
    VK_KHR_SWAPCHAIN_EXTENSION_NAME};

#ifdef NDEBUG
const bool enableValidationLayers = false;
#else
const bool enableValidationLayers = true;
#endif

VkResult CreateDebugUtilsMessengerEXT(VkInstance instance, const VkDebugUtilsMessengerCreateInfoEXT *pCreateInfo, const VkAllocationCallbacks *pAllocator, VkDebugUtilsMessengerEXT *pDebugMessenger);

void DestroyDebugUtilsMessengerEXT(VkInstance instance, VkDebugUtilsMessengerEXT debugMessenger, const VkAllocationCallbacks *pAllocator);

class ExampleApplication
{
public:
    void run()
    {
        initWindow();
        initVulkan();
        mainLoop();
        cleanup();
    }

private:
    GLFWwindow *window;

    // Scene management
    std::shared_ptr<SceneManager> sceneManager;
    
    // Renderer
    std::unique_ptr<VulkanRenderer> renderer;

    // Quaternions for SLERP rotation (for interactive model instance)
    glm::quat currentRotation = glm::quat(1.0f, 0.0f, 0.0f, 0.0f); // Identity quaternion
    glm::quat targetRotation = glm::quat(1.0f, 0.0f, 0.0f, 0.0f);  // Identity quaternion

    // Accumulated rotation from mouse dragging
    glm::quat accumulatedRotation = glm::quat(1.0f, 0.0f, 0.0f, 0.0f);

    // SLERP speed
    const float slerpSpeed = 5.0f;

    // Mouse state
    bool mousePressed = false;
    double lastMouseX = 0.0;
    double lastMouseY = 0.0;
    const float rotationSpeed = 0.005f;

    VkInstance instance;
    VkDebugUtilsMessengerEXT debugMessenger;
    VkSurfaceKHR surface;

    VkPhysicalDevice physicalDevice = VK_NULL_HANDLE;
    VkSampleCountFlagBits msaaSamples = VK_SAMPLE_COUNT_64_BIT;
    VkDevice device;

    VkQueue graphicsQueue;
    VkQueue presentQueue;

    void initWindow();

    static void mouseButtonCallback(GLFWwindow *window, int button, int action, int mods);

    static void cursorPosCallback(GLFWwindow *window, double xpos, double ypos);

    void handleMouseButton(int button, int action, int mods);

    void handleCursorPos(double xpos, double ypos);

    static void framebufferResizeCallback(GLFWwindow *window, int width, int height);

    void initVulkan();

    void mainLoop();

    void updateRotation(float deltaTime);

    void cleanup();

    void createInstance();

    void populateDebugMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT &createInfo);

    void setupDebugMessenger();

    void createSurface();

    void pickPhysicalDevice();

    void createLogicalDevice();

    void setupScene();

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
