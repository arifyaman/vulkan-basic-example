#pragma once

#include <glm/glm.hpp>
#include <string>

// PBR Material properties for physically-based rendering
struct Material
{
    // PBR Textures
    std::string baseColorTexture;         // Base color texture (RGBA)
    std::string metallicRoughnessTexture; // Metallic (B) / Roughness (G) texture
    std::string emissiveTexture;          // Emissive texture (RGB)

    // PBR Material Factors
    glm::vec4 baseColorFactor;    // Base color multiplier (RGBA) - default (1,1,1,1)
    glm::vec3 emissiveFactor;     // Emissive multiplier (RGB) - default (0,0,0)
    glm::vec4 fogColor;           // Fog color (RGBA) - default (1.0,1.0,1.0,1.0)
    float metallic;               // Metallic factor (0.0 = dielectric, 1.0 = metal) - default 0.0
    float roughness;              // Roughness factor (0.0 = smooth, 1.0 = rough) - default 0.5

    // Default constructor - white diffuse material
    Material()
        : baseColorFactor(1.0f, 1.0f, 1.0f, 1.0f),
          emissiveFactor(0.0f, 0.0f, 0.0f),
          fogColor(1.0f, 1.0f, 1.0f, 1.0f),
          metallic(0.0f),
          roughness(0.5f)
    {}

    // Constructor with base color
    Material(const glm::vec4& baseColor)
        : baseColorFactor(baseColor),
          emissiveFactor(0.0f, 0.0f, 0.0f),
          fogColor(1.0f, 1.0f, 1.0f, 1.0f),
          metallic(0.0f),
          roughness(0.5f)
    {}

    // Constructor with base color texture
    Material(const std::string& texturePath)
        : baseColorTexture(texturePath),
          baseColorFactor(1.0f, 1.0f, 1.0f, 1.0f),
          emissiveFactor(0.0f, 0.0f, 0.0f),
          fogColor(1.0f, 1.0f, 1.0f, 1.0f),
          metallic(0.0f),
          roughness(0.5f)
    {}

    // Full PBR constructor
    Material(const std::string& baseColorTex,
             const std::string& metallicRoughnessTex,
             const std::string& emissiveTex,
             const glm::vec4& baseColor = glm::vec4(1.0f),
             const glm::vec3& emissive = glm::vec3(0.0f),
             const glm::vec4& fog = glm::vec4(0.5f, 0.7f, 1.0f, 1.0f),
             float metallicVal = 0.0f,
             float roughnessVal = 0.5f)
        : baseColorTexture(baseColorTex),
          metallicRoughnessTexture(metallicRoughnessTex),
          emissiveTexture(emissiveTex),
          baseColorFactor(baseColor),
          emissiveFactor(emissive),
          fogColor(fog),
          metallic(metallicVal),
          roughness(roughnessVal)
    {}

    // PBR Material presets
    static Material diffuse(const glm::vec4& color = glm::vec4(1.0f))
    {
        return Material(color);
    }

    static Material textured(const std::string& texturePath)
    {
        return Material(texturePath);
    }

    static Material metal(const glm::vec4& color = glm::vec4(0.8f, 0.8f, 0.8f, 1.0f))
    {
        Material mat;
        mat.baseColorFactor = color;
        mat.metallic = 1.0f;  // Full metal
        mat.roughness = 0.3f; // Slightly rough for realism
        return mat;
    }

    static Material emissive(const glm::vec3& emissiveColor, const glm::vec4& baseColor = glm::vec4(0.0f))
    {
        Material mat;
        mat.baseColorFactor = baseColor;
        mat.emissiveFactor = emissiveColor;
        return mat;
    }
};
