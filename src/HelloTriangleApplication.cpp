// HelloTriangleApplication.cpp - Main application logic
#include "headers/HelloTriangleApplication.h"

void HelloTriangleApplication::initVulkan()
{
    // Initialize scene manager
    sceneManager = std::make_shared<SceneManager>();

    createInstance();
    setupDebugMessenger();
    createSurface();
    pickPhysicalDevice();
    createLogicalDevice();
    
    // Create renderer
    renderer = std::make_unique<VulkanRenderer>(physicalDevice, device, graphicsQueue, 
                                                 presentQueue, surface, window, msaaSamples);
    renderer->initialize();
    
    // Load shaders needed by this application
    renderer->createGraphicsPipelines({"shader", "custom"});
    
    setupScene();
}

void HelloTriangleApplication::setupScene()
{
    // Get command pool from renderer
    VkCommandPool commandPool = renderer->getCommandPool();
    
    // Create box model (using default 1x1x1 cube)
    auto cubeModel = std::shared_ptr<Model>(Model::createBox(device, physicalDevice, commandPool, graphicsQueue));
    sceneManager->setModel("cube", cubeModel);

    // Create multiple instances of the cube with different scales and positions
    auto instance1 = std::make_shared<ModelInstance>("cube", "instance_1");
    instance1->setPosition(glm::vec3(-1.5f, 0.0f, 0.0f));
    instance1->setScale(glm::vec3(2.0f, 2.0f, 2.0f)); // Smaller scale, left
    instance1->setMaterial(Material::metal(glm::vec4(1.0f, 0.0f, 0.0f, 1.0f))); // Red metal
    sceneManager->addInstance(instance1);

    auto instance2 = std::make_shared<ModelInstance>("cube", "instance_2");
    instance2->setPosition(glm::vec3(0.0f, 0.0f, 0.0f));
    instance2->setScale(glm::vec3(1.0f, 1.0f, 1.0f)); // Normal scale, center
    // Use custom shader that interpolates between green and yellow
    instance2->setMaterial(Material::colorLerp(
        glm::vec4(0.0f, 1.0f, 0.0f, 1.0f),  // Color 1: Green
        glm::vec4(1.0f, 1.0f, 0.0f, 1.0f)   // Color 2: Yellow
    ));
    sceneManager->addInstance(instance2);

    auto instance3 = std::make_shared<ModelInstance>("cube", "instance_3");
    instance3->setPosition(glm::vec3(1.5f, 0.0f, 0.0f));
    instance3->setScale(glm::vec3(1.5f, 1.5f, 1.5f)); // Larger scale, right
    instance3->setMaterial(Material::metal(glm::vec4(0.0f, 0.0f, 1.0f, 1.0f))); // Blue metal
    sceneManager->addInstance(instance3);
}

void HelloTriangleApplication::mainLoop()
{
    auto lastTime = std::chrono::high_resolution_clock::now();

    while (!glfwWindowShouldClose(window))
    {
        auto currentTime = std::chrono::high_resolution_clock::now();
        float deltaTime = std::chrono::duration<float, std::chrono::seconds::period>(currentTime - lastTime).count();
        lastTime = currentTime;

        glfwPollEvents();
        updateRotation(deltaTime);

        // Update the rotation of the first instance
        if (auto instance = sceneManager->getInstance(0)) {
            instance->setRotation(currentRotation);
        }

        renderer->drawFrame(sceneManager);
    }

    vkDeviceWaitIdle(device);
}

void HelloTriangleApplication::cleanup()
{
    // Cleanup renderer (this will clean up all rendering resources)
    renderer.reset();
    
    // Cleanup all models
    sceneManager->cleanupModels(device);

    vkDestroyDevice(device, nullptr);

    if (enableValidationLayers)
    {
        DestroyDebugUtilsMessengerEXT(instance, debugMessenger, nullptr);
    }

    vkDestroySurfaceKHR(instance, surface, nullptr);
    vkDestroyInstance(instance, nullptr);

    glfwDestroyWindow(window);

    glfwTerminate();
}

void HelloTriangleApplication::updateRotation(float deltaTime)
{
    // Calculate SLERP factor (clamped between 0 and 1)
    float t = glm::clamp(slerpSpeed * deltaTime, 0.0f, 1.0f);

    // Use SLERP for smooth rotation
    if (t > 0.0f)
    {
        currentRotation = glm::slerp(currentRotation, targetRotation, t);

        // Normalize to prevent drift
        currentRotation = glm::normalize(currentRotation);
    }
}
