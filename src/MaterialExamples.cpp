// Example: Using the Dynamic Shader Parameter System
// This file demonstrates various ways to create and use materials with the new parameter system

#include "headers/Material.h"
#include <glm/glm.hpp>

// Example 1: Basic material with scalar parameters
Material createMetallicMaterial()
{
    Material mat("pbr", glm::vec4(0.8f, 0.8f, 0.8f, 1.0f));
    mat.setParamScalar("metallic", 0.95f);
    mat.setParamScalar("roughness", 0.2f);
    mat.setParamScalar("ao", 1.0f);  // Ambient occlusion
    return mat;
}

// Example 2: Material with vector parameters
Material createGlowingMaterial()
{
    Material mat("emissive", glm::vec4(0.2f, 0.2f, 0.8f, 1.0f));
    mat.setParamVec3("emissiveColor", glm::vec3(0.0f, 0.5f, 1.0f));
    mat.setParamScalar("emissiveIntensity", 2.5f);
    mat.setParamVec2("pulseSpeed", glm::vec2(1.0f, 0.5f));
    return mat;
}

// Example 3: Animated material
Material createWaterMaterial()
{
    Material mat("water", glm::vec4(0.1f, 0.3f, 0.6f, 0.8f));
    
    // Wave parameters
    mat.setParamVec2("waveDirection1", glm::vec2(1.0f, 0.0f));
    mat.setParamVec2("waveDirection2", glm::vec2(0.5f, 0.866f));
    mat.setParamScalar("waveSpeed1", 0.8f);
    mat.setParamScalar("waveSpeed2", 1.2f);
    mat.setParamScalar("waveAmplitude", 0.05f);
    
    // Color parameters
    mat.setParamVec3("shallowColor", glm::vec3(0.3f, 0.7f, 0.9f));
    mat.setParamVec3("deepColor", glm::vec3(0.0f, 0.1f, 0.3f));
    mat.setParamScalar("depthFalloff", 2.0f);
    
    // Reflection/refraction
    mat.setParamScalar("fresnelPower", 3.0f);
    mat.setParamScalar("refractiveIndex", 1.33f);
    
    return mat;
}

// Example 4: Gradient material with multiple colors
Material createRainbowMaterial()
{
    Material mat("gradient", glm::vec4(1.0f, 0.0f, 0.0f, 1.0f));
    
    // Multiple color stops
    mat.setParamVec3("color1", glm::vec3(1.0f, 0.0f, 0.0f));  // Red
    mat.setParamVec3("color2", glm::vec3(1.0f, 0.5f, 0.0f));  // Orange
    mat.setParamVec3("color3", glm::vec3(1.0f, 1.0f, 0.0f));  // Yellow
    mat.setParamVec3("color4", glm::vec3(0.0f, 1.0f, 0.0f));  // Green
    
    // Gradient control
    mat.setParamScalar("gradientSpeed", 0.5f);
    mat.setParamVec2("gradientDirection", glm::vec2(0.0f, 1.0f));
    
    return mat;
}

// Example 5: Textured material with parameters
Material createDetailedMaterial()
{
    Material mat("textured", glm::vec4(1.0f, 1.0f, 1.0f, 1.0f));
    
    // Texture bindings (these would correspond to descriptor set bindings)
    mat.setParamTexture("albedoMap", 0);
    mat.setParamTexture("normalMap", 1);
    mat.setParamTexture("roughnessMap", 2);
    mat.setParamTexture("aoMap", 3);
    
    // Texture parameters
    mat.setParamVec2("uvScale", glm::vec2(2.0f, 2.0f));
    mat.setParamVec2("uvOffset", glm::vec2(0.0f, 0.0f));
    mat.setParamScalar("normalStrength", 1.0f);
    
    // Material multipliers
    mat.setParamVec3("colorTint", glm::vec3(1.0f, 0.95f, 0.9f));
    mat.setParamScalar("roughnessMultiplier", 1.0f);
    
    return mat;
}

// Example 6: Procedural material
Material createProceduralMaterial()
{
    Material mat("procedural", glm::vec4(0.5f, 0.5f, 0.5f, 1.0f));
    
    // Noise parameters
    mat.setParamVec3("noiseScale", glm::vec3(2.0f, 2.0f, 2.0f));
    mat.setParamScalar("noiseOctaves", 4.0f);
    mat.setParamScalar("noisePersistence", 0.5f);
    mat.setParamScalar("noiseLacunarity", 2.0f);
    
    // Pattern parameters
    mat.setParamVec2("patternFrequency", glm::vec2(10.0f, 10.0f));
    mat.setParamScalar("patternAmplitude", 0.3f);
    
    // Color mapping
    mat.setParamVec3("lowColor", glm::vec3(0.1f, 0.1f, 0.1f));
    mat.setParamVec3("highColor", glm::vec3(0.9f, 0.9f, 0.9f));
    
    return mat;
}

// Example 7: Hologram material
Material createHologramMaterial()
{
    Material mat("hologram", glm::vec4(0.0f, 1.0f, 1.0f, 0.7f));
    
    // Hologram effect
    mat.setParamVec3("holoColor", glm::vec3(0.0f, 1.0f, 1.0f));
    mat.setParamScalar("scanlineFrequency", 50.0f);
    mat.setParamScalar("scanlineSpeed", 2.0f);
    mat.setParamScalar("scanlineIntensity", 0.3f);
    
    // Flicker
    mat.setParamScalar("flickerSpeed", 15.0f);
    mat.setParamScalar("flickerAmount", 0.1f);
    
    // Distortion
    mat.setParamVec2("distortionAmount", glm::vec2(0.02f, 0.02f));
    mat.setParamScalar("distortionSpeed", 1.0f);
    
    return mat;
}

// Example 8: Using preset materials with custom parameters
Material createCustomPreset()
{
    // Start with a preset
    Material mat = Material::colorLerp(
        glm::vec4(1.0f, 0.0f, 0.0f, 1.0f),
        glm::vec4(0.0f, 0.0f, 1.0f, 1.0f)
    );
    
    // Add additional custom parameters
    mat.setParamScalar("lerpSpeed", 0.5f);
    mat.setParamScalar("lerpPulse", 0.2f);
    mat.setParamVec2("lerpDirection", glm::vec2(1.0f, 0.0f));
    
    return mat;
}

// Example 9: Backward compatibility with legacy system
Material createLegacyCompatibleMaterial()
{
    Material mat("custom", glm::vec4(1.0f, 1.0f, 1.0f, 1.0f));
    
    // This still works (legacy method)
    mat.setCustomParam("color2", glm::vec4(0.5f, 0.5f, 0.5f, 1.0f));
    
    // New method (recommended)
    mat.setParamVec4("color3", glm::vec4(0.2f, 0.2f, 0.2f, 1.0f));
    mat.setParamScalar("blend", 0.5f);
    
    return mat;
}

// Example 10: Complex multi-parameter material
Material createComplexMaterial()
{
    Material mat("complex", glm::vec4(0.7f, 0.7f, 0.7f, 1.0f));
    
    // Base properties
    mat.setParamVec3("baseColor", glm::vec3(0.7f, 0.7f, 0.7f));
    mat.setParamScalar("metallic", 0.5f);
    mat.setParamScalar("roughness", 0.4f);
    
    // Subsurface scattering
    mat.setParamVec3("subsurfaceColor", glm::vec3(1.0f, 0.8f, 0.7f));
    mat.setParamScalar("subsurfaceRadius", 0.5f);
    
    // Clearcoat
    mat.setParamScalar("clearcoat", 1.0f);
    mat.setParamScalar("clearcoatRoughness", 0.1f);
    
    // Anisotropic
    mat.setParamScalar("anisotropic", 0.8f);
    mat.setParamVec2("anisotropicDirection", glm::vec2(1.0f, 0.0f));
    
    // Sheen
    mat.setParamVec3("sheenColor", glm::vec3(1.0f, 0.9f, 0.8f));
    mat.setParamScalar("sheenRoughness", 0.3f);
    
    return mat;
}
