// Example: Using the New Model Architecture

// ============================================================================
// EXAMPLE 1: Basic Usage (Current Implementation)
// ============================================================================

// The new architecture is used automatically in HelloTriangleApplication:
// 
// 1. loadModel() now does:
//    - Creates a Model and loads geometry
//    - Adds it to SceneManager
//    - Creates one ModelInstance
//
// 2. Recording command buffers iterates over instances:
//    for (size_t i = 0; i < sceneManager->getInstanceCount(); ++i)
//    {
//        auto instance = sceneManager->getInstance(i);
//        vkCmdDrawIndexed(...);  // Render this instance
//    }

// ============================================================================
// EXAMPLE 2: Adding Multiple Instances at Runtime
// ============================================================================

// In HelloTriangleApplication::loadModel() or any other function:

void loadModel()
{
    // Load the model (shared by all instances)
    vikingRoomModel = std::make_shared<Model>();
    vikingRoomModel->loadFromFile(MODEL_PATH);
    sceneManager->setModel("viking_room", vikingRoomModel);

    // Create main instance
    auto mainInstance = std::make_shared<ModelInstance>("viking_room_0");
    mainInstance->setPosition(glm::vec3(0.0f, 0.0f, 0.0f));
    sceneManager->addInstance(mainInstance);

    // Create a second instance (duplicate at different position)
    auto instance2 = std::make_shared<ModelInstance>("viking_room_1");
    instance2->setPosition(glm::vec3(3.0f, 0.0f, 0.0f));
    instance2->setScale(glm::vec3(0.8f, 0.8f, 0.8f));
    sceneManager->addInstance(instance2);

    // Create a third instance (rotated and scaled)
    auto instance3 = std::make_shared<ModelInstance>("viking_room_2");
    instance3->setPosition(glm::vec3(-3.0f, 0.0f, 0.0f));
    instance3->setRotation(glm::angleAxis(glm::radians(45.0f), glm::vec3(0.0f, 1.0f, 0.0f)));
    instance3->setScale(glm::vec3(1.2f, 1.2f, 1.2f));
    sceneManager->addInstance(instance3);
}

// ============================================================================
// EXAMPLE 3: Modifying Instance Transforms at Runtime
// ============================================================================

void updateFrame()
{
    // Get a specific instance and modify it
    auto instance = sceneManager->getInstance(0);
    if (instance)
    {
        // Rotate it over time
        static float angle = 0.0f;
        angle += 0.016f;  // ~60fps
        glm::quat rotation = glm::angleAxis(angle, glm::vec3(0.0f, 1.0f, 0.0f));
        instance->setRotation(rotation);

        // Or modify position
        instance->translate(glm::vec3(0.01f, 0.0f, 0.0f));
    }
}

// ============================================================================
// EXAMPLE 4: Future - Per-Instance Rendering (Pseudocode)
// ============================================================================

// This is how you would implement per-instance uniform buffers:

void recordCommandBuffer(VkCommandBuffer commandBuffer, uint32_t imageIndex)
{
    // ... setup code ...

    // For each instance, update its own uniform buffer
    for (size_t i = 0; i < sceneManager->getInstanceCount(); ++i)
    {
        auto instance = sceneManager->getInstance(i);
        if (instance)
        {
            // Create or update per-instance UBO
            UniformBufferObject ubo{};
            ubo.model = instance->getModelMatrix();  // Per-instance transform!
            ubo.view = mainViewMatrix;
            ubo.proj = projectionMatrix;

            // Update the uniform buffer for this instance
            void* data = nullptr;
            vkMapMemory(device, uniformBuffersMemory[i], 0, sizeof(ubo), 0, &data);
            memcpy(data, &ubo, sizeof(ubo));
            vkUnmapMemory(device, uniformBuffersMemory[i]);

            // Bind the descriptor set for this instance
            vkCmdBindDescriptorSets(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS,
                                    pipelineLayout, 0, 1, &descriptorSets[i], 0, nullptr);

            // Draw this instance
            vkCmdDrawIndexed(commandBuffer, indexCount, 1, 0, 0, 0);
        }
    }
}

// ============================================================================
// EXAMPLE 5: Loading Multiple Different Models
// ============================================================================

void initScene()
{
    // Load first model
    auto vikingModel = std::make_shared<Model>();
    vikingModel->loadFromFile("models/viking_room.obj");
    sceneManager->setModel("viking", vikingModel);

    // Load second model
    auto cubeModel = std::make_shared<Model>();
    cubeModel->loadFromFile("models/cube.obj");
    sceneManager->setModel("cube", cubeModel);

    // Create instances of different models
    auto viking = std::make_shared<ModelInstance>("viking_1");
    viking->setPosition(glm::vec3(-5.0f, 0.0f, 0.0f));
    sceneManager->addInstance(viking);

    auto cube = std::make_shared<ModelInstance>("cube_1");
    cube->setPosition(glm::vec3(5.0f, 0.0f, 0.0f));
    cube->setScale(glm::vec3(0.5f, 0.5f, 0.5f));
    sceneManager->addInstance(cube);
}

// Note: Currently, the rendering system uses a single vertex/index buffer
// and renders all instances with the same geometry. To support multiple
// different models, you would need to:
//
// 1. Store multiple vertex/index buffers (one per model)
// 2. Track which instance uses which model
// 3. Bind appropriate buffers before each draw call
// 4. Update indexCount based on current instance's model
