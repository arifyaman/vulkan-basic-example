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
    renderer->createGraphicsPipelines({"shader", "custom"});

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
    auto instance1 = std::make_shared<ModelInstance>("cube", "instance_1");
    instance1->setPosition(glm::vec3(-1.5f, 0.0f, 0.0f));
    instance1->setScale(glm::vec3(2.0f, 2.0f, 2.0f)); // Smaller scale, left
    instance1->setMaterial(Material::metal(glm::vec4(1.0f, 0.0f, 0.0f, 1.0f))); // Red metal
    sceneManager->addInstance(instance1);

    auto instance2 = std::make_shared<ModelInstance>("cube", "instance_2");
    instance2->setPosition(glm::vec3(0.0f, 0.0f, 0.0f));
    instance2->setScale(glm::vec3(1.0f, 1.0f, 1.0f)); // Normal scale, center
    // Use texture material
    Material textureMat("shader", "textures/viking_room.png");
    textureMat.specularColor = glm::vec3(0.1f, 0.1f, 0.1f);
    textureMat.shininess = 32.0f;
    instance2->setMaterial(textureMat);
    sceneManager->addInstance(instance2);

    auto instance3 = std::make_shared<ModelInstance>("cube", "instance_3");
    instance3->setPosition(glm::vec3(1.5f, 0.0f, 0.0f));
    instance3->setScale(glm::vec3(1.5f, 1.5f, 1.5f)); // Larger scale, right
    instance3->setMaterial(Material::metal(glm::vec4(0.0f, 0.0f, 1.0f, 1.0f))); // Blue metal
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
