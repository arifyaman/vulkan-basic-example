#pragma once

#include <glm/glm.hpp>
#include <string>
#include <variant>
#include <vector>
#include <cstring>

// Types of shader parameters that can be passed to shaders
enum class ShaderParameterType
{
    Scalar,   // float
    Vector2,  // vec2
    Vector3,  // vec3
    Vector4,  // vec4
    Texture   // Texture reference (descriptor set binding)
};

// A flexible shader parameter that can hold different types
class ShaderParameter
{
public:
    using ValueType = std::variant<float, glm::vec2, glm::vec3, glm::vec4, uint32_t>;

private:
    std::string name;
    ShaderParameterType type;
    ValueType value;

public:
    // Constructors for different types
    ShaderParameter() : name(""), type(ShaderParameterType::Scalar), value(0.0f) {}

    ShaderParameter(const std::string& paramName, float scalar)
        : name(paramName), type(ShaderParameterType::Scalar), value(scalar) {}

    ShaderParameter(const std::string& paramName, const glm::vec2& vec)
        : name(paramName), type(ShaderParameterType::Vector2), value(vec) {}

    ShaderParameter(const std::string& paramName, const glm::vec3& vec)
        : name(paramName), type(ShaderParameterType::Vector3), value(vec) {}

    ShaderParameter(const std::string& paramName, const glm::vec4& vec)
        : name(paramName), type(ShaderParameterType::Vector4), value(vec) {}

    // Texture constructor (stores texture binding index)
    static ShaderParameter texture(const std::string& paramName, uint32_t textureBinding)
    {
        ShaderParameter param;
        param.name = paramName;
        param.type = ShaderParameterType::Texture;
        param.value = textureBinding;
        return param;
    }

    // Getters
    const std::string& getName() const { return name; }
    ShaderParameterType getType() const { return type; }

    float getScalar() const
    {
        return type == ShaderParameterType::Scalar ? std::get<float>(value) : 0.0f;
    }

    glm::vec2 getVec2() const
    {
        return type == ShaderParameterType::Vector2 ? std::get<glm::vec2>(value) : glm::vec2(0.0f);
    }

    glm::vec3 getVec3() const
    {
        return type == ShaderParameterType::Vector3 ? std::get<glm::vec3>(value) : glm::vec3(0.0f);
    }

    glm::vec4 getVec4() const
    {
        return type == ShaderParameterType::Vector4 ? std::get<glm::vec4>(value) : glm::vec4(0.0f);
    }

    uint32_t getTextureBinding() const
    {
        return type == ShaderParameterType::Texture ? std::get<uint32_t>(value) : 0;
    }

    // Get the size in bytes for push constants (aligned to vec4 boundaries)
    size_t getPushConstantSize() const
    {
        switch (type)
        {
            case ShaderParameterType::Scalar:
                return sizeof(float);
            case ShaderParameterType::Vector2:
                return sizeof(glm::vec2);
            case ShaderParameterType::Vector3:
                // vec3 is padded to vec4 size in push constants
                return sizeof(glm::vec4);
            case ShaderParameterType::Vector4:
                return sizeof(glm::vec4);
            case ShaderParameterType::Texture:
                return 0; // Textures don't use push constants
            default:
                return 0;
        }
    }

    // Write parameter data to a buffer for push constants
    void writeToPushConstant(void* buffer) const
    {
        switch (type)
        {
            case ShaderParameterType::Scalar:
            {
                float f = std::get<float>(value);
                memcpy(buffer, &f, sizeof(float));
                break;
            }
            case ShaderParameterType::Vector2:
            {
                glm::vec2 v = std::get<glm::vec2>(value);
                memcpy(buffer, &v, sizeof(glm::vec2));
                break;
            }
            case ShaderParameterType::Vector3:
            {
                glm::vec3 v = std::get<glm::vec3>(value);
                // Pad vec3 to vec4 for alignment
                glm::vec4 padded(v, 0.0f);
                memcpy(buffer, &padded, sizeof(glm::vec4));
                break;
            }
            case ShaderParameterType::Vector4:
            {
                glm::vec4 v = std::get<glm::vec4>(value);
                memcpy(buffer, &v, sizeof(glm::vec4));
                break;
            }
            case ShaderParameterType::Texture:
                // Textures don't write to push constants
                break;
        }
    }
};

// Container for managing multiple shader parameters
class ShaderParameterSet
{
private:
    std::vector<ShaderParameter> parameters;

public:
    ShaderParameterSet() = default;

    // Add parameters
    void add(const ShaderParameter& param)
    {
        // Check if parameter already exists and update it
        for (auto& p : parameters)
        {
            if (p.getName() == param.getName())
            {
                p = param;
                return;
            }
        }
        parameters.push_back(param);
    }

    void addScalar(const std::string& name, float value)
    {
        add(ShaderParameter(name, value));
    }

    void addVec2(const std::string& name, const glm::vec2& value)
    {
        add(ShaderParameter(name, value));
    }

    void addVec3(const std::string& name, const glm::vec3& value)
    {
        add(ShaderParameter(name, value));
    }

    void addVec4(const std::string& name, const glm::vec4& value)
    {
        add(ShaderParameter(name, value));
    }

    void addTexture(const std::string& name, uint32_t binding)
    {
        add(ShaderParameter::texture(name, binding));
    }

    // Get parameter by name
    const ShaderParameter* getParameter(const std::string& name) const
    {
        for (const auto& p : parameters)
        {
            if (p.getName() == name)
                return &p;
        }
        return nullptr;
    }

    // Get all parameters
    const std::vector<ShaderParameter>& getAll() const
    {
        return parameters;
    }

    // Calculate total size needed for push constants
    size_t getTotalPushConstantSize() const
    {
        size_t total = 0;
        for (const auto& param : parameters)
        {
            if (param.getType() != ShaderParameterType::Texture)
            {
                total += param.getPushConstantSize();
            }
        }
        return total;
    }

    // Clear all parameters
    void clear()
    {
        parameters.clear();
    }

    size_t size() const
    {
        return parameters.size();
    }

    bool empty() const
    {
        return parameters.empty();
    }
};
