#pragma once

#include <glm/glm.hpp>
#include <string>
#include <unordered_map>
#include "ShaderParameter.h"

// Material properties for rendering
struct Material
{
    std::string shaderName;       // Shader to use (e.g., "shader", "custom")
    glm::vec4 baseColor;          // Diffuse/albedo color (RGBA) - used when no texture
    glm::vec3 specularColor;      // Specular reflection color (RGB)
    float shininess;              // Specular shininess/glossiness
    std::string diffuseTexture;   // Path to diffuse texture file (empty = use baseColor)

    // Modern flexible shader parameters system
    ShaderParameterSet customParams;

    // Legacy storage for backward compatibility (deprecated)
    std::unordered_map<std::string, glm::vec4> legacyParams;

    Material()
        : shaderName("shader"),
          baseColor(1.0f, 1.0f, 1.0f, 1.0f),
          specularColor(1.0f, 1.0f, 1.0f),
          shininess(32.0f)
    {
        // Initialize default shader params
        customParams.addVec4("specularData", glm::vec4(specularColor, shininess));
    }

    Material(const std::string &shader, const glm::vec4 &base, const glm::vec3 &specular = glm::vec3(1.0f), float shine = 32.0f)
        : shaderName(shader),
          baseColor(base),
          specularColor(specular),
          shininess(shine)
    {
        // Initialize shader params based on shader type
        if (shader == "shader")
        {
            customParams.addVec4("specularData", glm::vec4(specularColor, shininess));
        }
    }

    // Constructor with diffuse texture
    Material(const std::string &shader, const std::string &texturePath, const glm::vec3 &specular = glm::vec3(1.0f), float shine = 32.0f)
        : shaderName(shader),
          baseColor(1.0f, 1.0f, 1.0f, 1.0f), // White base color when using texture
          specularColor(specular),
          shininess(shine),
          diffuseTexture(texturePath)
    {
        // Initialize shader params based on shader type
        if (shader == "shader")
        {
            customParams.addVec4("specularData", glm::vec4(specularColor, shininess));
        }
    }

    // Legacy constructor for backward compatibility
    Material(const glm::vec4 &base, const glm::vec3 &specular = glm::vec3(1.0f), float shine = 32.0f)
        : shaderName("shader"),
          baseColor(base),
          specularColor(specular),
          shininess(shine)
    {
        // Initialize default shader params
        customParams.addVec4("specularData", glm::vec4(specularColor, shininess));
    }
    
    // Modern parameter setters
    void setParam(const ShaderParameter& param)
    {
        customParams.add(param);
    }
    
    void setParamScalar(const std::string& name, float value)
    {
        customParams.addScalar(name, value);
    }
    
    void setParamVec2(const std::string& name, const glm::vec2& value)
    {
        customParams.addVec2(name, value);
    }
    
    void setParamVec3(const std::string& name, const glm::vec3& value)
    {
        customParams.addVec3(name, value);
    }
    
    void setParamVec4(const std::string& name, const glm::vec4& value)
    {
        customParams.addVec4(name, value);
    }
    
    void setParamTexture(const std::string& name, uint32_t binding)
    {
        customParams.addTexture(name, binding);
    }

    // Convenience method for setting diffuse texture
    void setDiffuseTexture(const std::string& texturePath)
    {
        diffuseTexture = texturePath;
    }

    // Check if material has a diffuse texture
    bool hasDiffuseTexture() const
    {
        return !diffuseTexture.empty();
    }
    
    // Legacy setters for backward compatibility (deprecated)
    void setCustomParam(const std::string &name, const glm::vec4 &value)
    {
        legacyParams[name] = value;
        customParams.addVec4(name, value);
    }
    
    // Get custom shader parameter (returns zero vec4 if not found)
    glm::vec4 getCustomParam(const std::string &name) const
    {
        // Try modern system first
        const ShaderParameter* param = customParams.getParameter(name);
        if (param && param->getType() == ShaderParameterType::Vector4)
        {
            return param->getVec4();
        }
        
        // Fall back to legacy system
        auto it = legacyParams.find(name);
        return (it != legacyParams.end()) ? it->second : glm::vec4(0.0f);
    }

    // Get shader-specific data for push constants
    // Material decides what data to provide based on its shader
    // Returns the custom parameter set for flexible rendering
    const ShaderParameterSet& getShaderParams() const
    {
        return customParams;
    }

    // Common material presets
    static Material plastic(const glm::vec4 &color)
    {
        return Material("shader", color, glm::vec3(0.5f, 0.5f, 0.5f), 32.0f);
    }

    static Material metal(const glm::vec4 &color)
    {
        return Material("shader", color, glm::vec3(1.0f, 1.0f, 1.0f), 128.0f);
    }

    static Material rubber(const glm::vec4 &color)
    {
        return Material("shader", color, glm::vec3(0.1f, 0.1f, 0.1f), 8.0f);
    }

    static Material matte(const glm::vec4 &color)
    {
        return Material("shader", color, glm::vec3(0.0f, 0.0f, 0.0f), 1.0f);
    }
    
    // Custom shader preset: Color interpolation
    static Material colorLerp(const glm::vec4 &color1, const glm::vec4 &color2)
    {
        Material mat("custom", color1, glm::vec3(0.0f), 1.0f);
        mat.setParamVec4("color2", color2);
        return mat;
    }
    
    // Advanced custom shader preset examples
    static Material gradientMaterial(const glm::vec3 &color1, const glm::vec3 &color2, float blend)
    {
        Material mat("custom", glm::vec4(color1, 1.0f));
        mat.setParamVec3("color2", color2);
        mat.setParamScalar("blendFactor", blend);
        return mat;
    }
    
    static Material animatedMaterial(const glm::vec4 &baseColor, float speed, float intensity)
    {
        Material mat("custom", baseColor);
        mat.setParamScalar("animSpeed", speed);
        mat.setParamScalar("animIntensity", intensity);
        return mat;
    }
};
