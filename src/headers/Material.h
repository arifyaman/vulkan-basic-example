#pragma once

#include <glm/glm.hpp>

// Material properties for rendering
struct Material
{
    glm::vec4 baseColor;      // Diffuse/albedo color (RGBA)
    glm::vec3 specularColor;  // Specular reflection color (RGB)
    float shininess;          // Specular shininess/glossiness

    Material()
        : baseColor(1.0f, 1.0f, 1.0f, 1.0f),
          specularColor(1.0f, 1.0f, 1.0f),
          shininess(32.0f)
    {
    }

    Material(const glm::vec4 &base, const glm::vec3 &specular = glm::vec3(1.0f), float shine = 32.0f)
        : baseColor(base),
          specularColor(specular),
          shininess(shine)
    {
    }

    // Common material presets
    static Material plastic(const glm::vec4 &color)
    {
        return Material(color, glm::vec3(0.5f, 0.5f, 0.5f), 32.0f);
    }

    static Material metal(const glm::vec4 &color)
    {
        return Material(color, glm::vec3(1.0f, 1.0f, 1.0f), 128.0f);
    }

    static Material rubber(const glm::vec4 &color)
    {
        return Material(color, glm::vec3(0.1f, 0.1f, 0.1f), 8.0f);
    }

    static Material matte(const glm::vec4 &color)
    {
        return Material(color, glm::vec3(0.0f, 0.0f, 0.0f), 1.0f);
    }
};
