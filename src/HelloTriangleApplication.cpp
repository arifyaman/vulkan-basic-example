// HelloTriangleApplication.cpp - Main application logic
#include "include/HelloTriangleApplication.h"

void HelloTriangleApplication::initVulkan()
{
    // Initialize scene manager
    sceneManager = std::make_shared<SceneManager>();

    createInstance();
    setupDebugMessenger();
    createSurface();
    pickPhysicalDevice();
    createLogicalDevice();
    createSwapChain();
    createImageViews();
    createRenderPass();
    createDescriptorSetLayout();
    createGraphicsPipeline();
    createCommandPool();
    createColorResources();
    createDepthResources();
    createFramebuffers();
    createTextureImage();
    createTextureImageView();
    createTextureSampler();
    setupScene();
    createUniformBuffers();
    createDescriptorPool();
    createDescriptorSets();
    createCommandBuffers();
    createSyncObjects();
}

void HelloTriangleApplication::setupScene()
{
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
    instance2->setMaterial(Material::metal(glm::vec4(0.0f, 1.0f, 0.0f, 1.0f))); // Green metal
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
        drawFrame();
    }

    vkDeviceWaitIdle(device);
}

void HelloTriangleApplication::cleanupSwapChain()
{
    vkDestroyImageView(device, depthImageView, nullptr);
    vkDestroyImage(device, depthImage, nullptr);
    vkFreeMemory(device, depthImageMemory, nullptr);

    vkDestroyImageView(device, colorImageView, nullptr);
    vkDestroyImage(device, colorImage, nullptr);
    vkFreeMemory(device, colorImageMemory, nullptr);

    for (auto framebuffer : swapChainFramebuffers)
    {
        vkDestroyFramebuffer(device, framebuffer, nullptr);
    }

    for (auto imageView : swapChainImageViews)
    {
        vkDestroyImageView(device, imageView, nullptr);
    }

    vkDestroySwapchainKHR(device, swapChain, nullptr);
}

void HelloTriangleApplication::cleanup()
{
    cleanupSwapChain();

    // Cleanup all models
    sceneManager->cleanupModels(device);

    vkDestroyPipeline(device, graphicsPipeline, nullptr);
    vkDestroyPipelineLayout(device, pipelineLayout, nullptr);
    vkDestroyRenderPass(device, renderPass, nullptr);

    for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++)
    {
        vkDestroyBuffer(device, uniformBuffers[i], nullptr);
        vkFreeMemory(device, uniformBuffersMemory[i], nullptr);
    }

    vkDestroyDescriptorPool(device, descriptorPool, nullptr);

    vkDestroySampler(device, textureSampler, nullptr);
    vkDestroyImageView(device, textureImageView, nullptr);

    vkDestroyImage(device, textureImage, nullptr);
    vkFreeMemory(device, textureImageMemory, nullptr);

    vkDestroyDescriptorSetLayout(device, descriptorSetLayout, nullptr);

    for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++)
    {
        vkDestroySemaphore(device, renderFinishedSemaphores[i], nullptr);
        vkDestroySemaphore(device, imageAvailableSemaphores[i], nullptr);
        vkDestroyFence(device, inFlightFences[i], nullptr);
    }

    vkDestroyCommandPool(device, commandPool, nullptr);

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

void HelloTriangleApplication::recreateSwapChain()
{
    int width = 0, height = 0;
    glfwGetFramebufferSize(window, &width, &height);
    while (width == 0 || height == 0)
    {
        glfwGetFramebufferSize(window, &width, &height);
        glfwWaitEvents();
    }

    vkDeviceWaitIdle(device);

    cleanupSwapChain();

    createSwapChain();
    createImageViews();
    createColorResources();
    createDepthResources();
    createFramebuffers();
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