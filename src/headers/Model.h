#pragma once

#include <glm/glm.hpp>
#include <vector>
#include <string>
#include <array>
#include <unordered_map>
#include <stdexcept>

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

struct Vertex
{
    glm::vec3 pos;
    glm::vec3 color;
    glm::vec2 texCoord;
    glm::vec3 normal;

    static VkVertexInputBindingDescription getBindingDescription()
    {
        VkVertexInputBindingDescription bindingDescription{};
        bindingDescription.binding = 0;
        bindingDescription.stride = sizeof(Vertex);
        bindingDescription.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

        return bindingDescription;
    }

    static std::array<VkVertexInputAttributeDescription, 4> getAttributeDescriptions()
    {
        std::array<VkVertexInputAttributeDescription, 4> attributeDescriptions{};

        attributeDescriptions[0].binding = 0;
        attributeDescriptions[0].location = 0;
        attributeDescriptions[0].format = VK_FORMAT_R32G32B32_SFLOAT;
        attributeDescriptions[0].offset = offsetof(Vertex, pos);

        attributeDescriptions[1].binding = 0;
        attributeDescriptions[1].location = 1;
        attributeDescriptions[1].format = VK_FORMAT_R32G32B32_SFLOAT;
        attributeDescriptions[1].offset = offsetof(Vertex, color);

        attributeDescriptions[2].binding = 0;
        attributeDescriptions[2].location = 2;
        attributeDescriptions[2].format = VK_FORMAT_R32G32_SFLOAT;
        attributeDescriptions[2].offset = offsetof(Vertex, texCoord);

        attributeDescriptions[3].binding = 0;
        attributeDescriptions[3].location = 3;
        attributeDescriptions[3].format = VK_FORMAT_R32G32B32_SFLOAT;
        attributeDescriptions[3].offset = offsetof(Vertex, normal);

        return attributeDescriptions;
    }

    bool operator==(const Vertex &other) const
    {
        return pos == other.pos && color == other.color && texCoord == other.texCoord && normal == other.normal;
    }
};

namespace std
{
    template <>
    struct hash<glm::vec3>
    {
        size_t operator()(glm::vec3 const &v) const noexcept
        {
            size_t h1 = hash<float>()(v.x);
            size_t h2 = hash<float>()(v.y);
            size_t h3 = hash<float>()(v.z);
            return h1 ^ (h2 << 1) ^ (h3 << 2);
        }
    };

    template <>
    struct hash<glm::vec2>
    {
        size_t operator()(glm::vec2 const &v) const noexcept
        {
            size_t h1 = hash<float>()(v.x);
            size_t h2 = hash<float>()(v.y);
            return h1 ^ (h2 << 1);
        }
    };

    template <>
    struct hash<Vertex>
    {
        size_t operator()(Vertex const &vertex) const noexcept
        {
            size_t h1 = hash<glm::vec3>()(vertex.pos);
            size_t h2 = hash<glm::vec3>()(vertex.color);
            size_t h3 = hash<glm::vec2>()(vertex.texCoord);
            return h1 ^ (h2 << 1) ^ (h3 << 2);
        }
    };
}

// Represents the geometry and texture of a 3D model
class Model
{
public:
    Model(VkDevice device, VkPhysicalDevice physicalDevice, VkCommandPool commandPool, VkQueue graphicsQueue);
    ~Model();

    // Load model from OBJ file
    void loadFromFile(const std::string &modelPath);
    
    // Create a box model with specified dimensions
    static Model* createBox(VkDevice device, VkPhysicalDevice physicalDevice, VkCommandPool commandPool, VkQueue graphicsQueue, float width = 1.0f, float height = 1.0f, float depth = 1.0f);

    // Create Vulkan buffers for the model
    void createBuffers(VkDevice device, VkPhysicalDevice physicalDevice, VkCommandPool commandPool, VkQueue graphicsQueue);

    // Cleanup Vulkan resources
    void cleanup(VkDevice device);

    // Getters
    const std::vector<Vertex> &getVertices() const { return vertices; }
    const std::vector<uint32_t> &getIndices() const { return indices; }
    uint32_t getVertexCount() const { return static_cast<uint32_t>(vertices.size()); }
    uint32_t getIndexCount() const { return static_cast<uint32_t>(indices.size()); }
    VkBuffer getVertexBuffer() const { return vertexBuffer; }
    VkBuffer getIndexBuffer() const { return indexBuffer; }

    bool isLoaded() const { return !vertices.empty() && !indices.empty(); }
    bool hasBuffers() const { return vertexBuffer != VK_NULL_HANDLE && indexBuffer != VK_NULL_HANDLE; }

private:
    std::vector<Vertex> vertices;
    std::vector<uint32_t> indices;

    // Vulkan buffers
    VkBuffer vertexBuffer = VK_NULL_HANDLE;
    VkDeviceMemory vertexBufferMemory = VK_NULL_HANDLE;
    VkBuffer indexBuffer = VK_NULL_HANDLE;
    VkDeviceMemory indexBufferMemory = VK_NULL_HANDLE;

    // Helper methods for buffer creation
    void createBuffer(VkDevice device, VkPhysicalDevice physicalDevice, VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties, VkBuffer &buffer, VkDeviceMemory &bufferMemory);
    void copyBuffer(VkDevice device, VkCommandPool commandPool, VkQueue graphicsQueue, VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize size);
    uint32_t findMemoryType(VkPhysicalDevice physicalDevice, uint32_t typeFilter, VkMemoryPropertyFlags properties);
};
