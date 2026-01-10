// WindowManager.cpp - Window creation and input handling
#include "headers/HelloTriangleApplication.h"

void HelloTriangleApplication::initWindow()
{
    glfwInit();

    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);

    window = glfwCreateWindow(WIDTH, HEIGHT, "Vulkan - Drag to Rotate", nullptr, nullptr);
    glfwSetWindowUserPointer(window, this);
    glfwSetFramebufferSizeCallback(window, framebufferResizeCallback);

    // Set up mouse callbacks
    glfwSetMouseButtonCallback(window, mouseButtonCallback);
    glfwSetCursorPosCallback(window, cursorPosCallback);
}

void HelloTriangleApplication::mouseButtonCallback(GLFWwindow *window, int button, int action, int mods)
{
    auto app = reinterpret_cast<HelloTriangleApplication *>(glfwGetWindowUserPointer(window));
    app->handleMouseButton(button, action, mods);
}

void HelloTriangleApplication::cursorPosCallback(GLFWwindow *window, double xpos, double ypos)
{
    auto app = reinterpret_cast<HelloTriangleApplication *>(glfwGetWindowUserPointer(window));
    app->handleCursorPos(xpos, ypos);
}

void HelloTriangleApplication::handleMouseButton(int button, int action, int mods)
{
    if (button == GLFW_MOUSE_BUTTON_LEFT)
    {
        if (action == GLFW_PRESS)
        {
            mousePressed = true;
            glfwGetCursorPos(window, &lastMouseX, &lastMouseY);
        }
        else if (action == GLFW_RELEASE)
        {
            mousePressed = false;
        }
    }
}

void HelloTriangleApplication::handleCursorPos(double xpos, double ypos)
{
    if (mousePressed)
    {
        double deltaX = xpos - lastMouseX;
        double deltaY = ypos - lastMouseY;

        // Apply X rotation to X axis, Y rotation to X axis (both in world space)
        glm::quat deltaRotation = glm::angleAxis(static_cast<float>(-deltaY * rotationSpeed), glm::vec3(1.0f, 0.0f, 0.0f)) *
                                  glm::angleAxis(static_cast<float>(-deltaX * rotationSpeed), glm::vec3(0.0f, 0.0f, 1.0f));

        // Accumulate the rotation
        accumulatedRotation = deltaRotation * accumulatedRotation;

        // Update target rotation
        targetRotation = accumulatedRotation;

        lastMouseX = xpos;
        lastMouseY = ypos;
    }
}

void HelloTriangleApplication::framebufferResizeCallback(GLFWwindow *window, int width, int height)
{
    auto app = reinterpret_cast<HelloTriangleApplication *>(glfwGetWindowUserPointer(window));
    if (app->renderer)
    {
        app->renderer->setFramebufferResized(true);
    }
}
