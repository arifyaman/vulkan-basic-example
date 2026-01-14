// ExampleApplication.cpp - Main application logic
#include "headers/ExampleApplication.h"

void ExampleApplication::initVulkan()
{
    // Initialize Vulkan
    vulkanInitializer = std::make_unique<VulkanInitializer>();
    vulkanInitializer->initialize(window);

    // Initialize scene manager
    sceneManager = std::make_shared<SceneManager>();

    // Create renderer
    renderer = std::make_unique<VulkanRenderer>(
        vulkanInitializer->getPhysicalDevice(),
        vulkanInitializer->getDevice(),
        vulkanInitializer->getGraphicsQueue(),
        vulkanInitializer->getPresentQueue(),
        vulkanInitializer->getSurface(),
        window,
        vulkanInitializer->getMsaaSamples()
    );
    renderer->initialize();

    // Load shaders needed by this application
    renderer->createGraphicsPipelines({"shader"});

    setupScene();

    // Load textures required by materials
    renderer->loadRequiredTextures(sceneManager);
}

void ExampleApplication::setupScene()
{
    // Get command pool from renderer
    VkCommandPool commandPool = renderer->getCommandPool();

    // Create box model (using default 1x1x1 cube)
    auto cubeModel = std::shared_ptr<Model>(Model::createBox(
        vulkanInitializer->getDevice(),
        vulkanInitializer->getPhysicalDevice(),
        commandPool,
        vulkanInitializer->getGraphicsQueue()
    ));
    sceneManager->setModel("cube", cubeModel);

    // Create multiple instances of the cube with different scales and positions
    
    // Left cube - Red metal
    auto instance1 = std::make_shared<ModelInstance>("cube", "instance_1");
    instance1->setPosition(glm::vec3(-1.5f, 0.0f, 0.0f));
    instance1->setScale(glm::vec3(2.0f, 2.0f, 2.0f));
    Material redMetal;
    redMetal.baseColorTexture = ""; // No texture, use color only
    redMetal.baseColorFactor = glm::vec4(1.0f, 0.0f, 0.0f, 1.0f); // Red
    redMetal.emissiveFactor = glm::vec3(0.0f, 0.0f, 0.0f);
    redMetal.metallic = 1.0f; // Full metal
    redMetal.roughness = 0.3f;
    instance1->setMaterial(redMetal);
    sceneManager->addInstance(instance1);

    // Center cube - Textured
    auto instance2 = std::make_shared<ModelInstance>("cube", "instance_2");
    instance2->setPosition(glm::vec3(0.0f, 0.0f, 0.0f));
    instance2->setScale(glm::vec3(1.0f, 1.0f, 1.0f));
    Material pbrMat;
    pbrMat.baseColorTexture = "textures/viking_room.png";
    pbrMat.baseColorFactor = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
    pbrMat.emissiveFactor = glm::vec3(0.0f, 0.0f, 0.0f);
    pbrMat.metallic = 0.0f; // Not metal, just textured
    pbrMat.roughness = 0.5f;
    instance2->setMaterial(pbrMat);
    sceneManager->addInstance(instance2);

    // Right cube - Blue metal
    auto instance3 = std::make_shared<ModelInstance>("cube", "instance_3");
    instance3->setPosition(glm::vec3(1.5f, 0.0f, 0.0f));
    instance3->setScale(glm::vec3(1.5f, 1.5f, 1.5f));
    Material blueMetal;
    blueMetal.baseColorTexture = ""; // No texture, use color only
    blueMetal.baseColorFactor = glm::vec4(0.0f, 0.0f, 1.0f, 1.0f); // Blue
    blueMetal.emissiveFactor = glm::vec3(0.0f, 0.0f, 0.0f);
    blueMetal.metallic = 1.0f; // Full metal
    blueMetal.roughness = 0.3f;
    instance3->setMaterial(blueMetal);
    sceneManager->addInstance(instance3);
}

void ExampleApplication::mainLoop()
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

    vkDeviceWaitIdle(vulkanInitializer->getDevice());
}

void ExampleApplication::cleanup()
{
    // Cleanup renderer (this will clean up all rendering resources)
    renderer.reset();

    // Cleanup all models
    sceneManager->cleanupModels(vulkanInitializer->getDevice());

    // Cleanup Vulkan (this will destroy device, surface, instance, etc.)
    vulkanInitializer->cleanup();

    glfwDestroyWindow(window);
    glfwTerminate();
}

void ExampleApplication::updateRotation(float deltaTime)
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
