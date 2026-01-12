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
#include "VulkanInitializer.h"

const uint32_t WIDTH = 800;
const uint32_t HEIGHT = 600;

const std::string MODEL_PATH = "models/viking_room.obj";
const std::string TEXTURE_PATH = "textures/viking_room.png";

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

    // Vulkan initialization
    std::unique_ptr<VulkanInitializer> vulkanInitializer;

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

    void setupScene();
};
