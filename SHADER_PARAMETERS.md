# Dynamic Shader Parameter System

## Overview

The new shader parameter system provides a flexible way to pass various types of data to shaders through materials. It supports:
- Scalars (float)
- 2D vectors (vec2)
- 3D vectors (vec3)
- 4D vectors (vec4)
- Texture references (for descriptor bindings)

## Basic Usage

### Creating Materials with Parameters

```cpp
// Example 1: Simple scalar parameter
Material mat("custom", glm::vec4(1.0f, 0.0f, 0.0f, 1.0f));
mat.setParamScalar("roughness", 0.5f);
mat.setParamScalar("metallic", 0.8f);

// Example 2: Vector parameters
Material mat2("custom", glm::vec4(1.0f));
mat2.setParamVec2("uvScale", glm::vec2(2.0f, 2.0f));
mat2.setParamVec3("emissiveColor", glm::vec3(1.0f, 0.5f, 0.0f));
mat2.setParamVec4("color2", glm::vec4(0.0f, 1.0f, 0.0f, 1.0f));

// Example 3: Texture parameter (for descriptor set binding)
Material mat3("custom", glm::vec4(1.0f));
mat3.setParamTexture("diffuseMap", 0);  // Binding 0
mat3.setParamTexture("normalMap", 1);   // Binding 1

// Example 4: Mixed parameters
Material mat4("custom", glm::vec4(1.0f, 1.0f, 1.0f, 1.0f));
mat4.setParamVec3("tintColor", glm::vec3(0.8f, 0.9f, 1.0f));
mat4.setParamScalar("intensity", 2.0f);
mat4.setParamVec2("offset", glm::vec2(0.1f, 0.2f));
```

### Using Preset Materials

```cpp
// Color lerp material (for gradients)
Material gradient = Material::colorLerp(
    glm::vec4(1.0f, 0.0f, 0.0f, 1.0f),  // Red
    glm::vec4(0.0f, 0.0f, 1.0f, 1.0f)   // Blue
);

// Gradient with custom blend
Material gradient2 = Material::gradientMaterial(
    glm::vec3(1.0f, 0.5f, 0.0f),  // Orange
    glm::vec3(0.5f, 0.0f, 1.0f),  // Purple
    0.7f                           // Blend factor
);

// Animated material
Material animated = Material::animatedMaterial(
    glm::vec4(0.0f, 1.0f, 0.5f, 1.0f),  // Cyan-ish
    1.5f,                                // Speed
    0.3f                                 // Intensity
);
```

## How It Works

### ShaderParameter Class

The `ShaderParameter` class uses `std::variant` to store different types:
- `float` for scalars
- `glm::vec2` for 2D vectors
- `glm::vec3` for 3D vectors
- `glm::vec4` for 4D vectors
- `uint32_t` for texture bindings

### ShaderParameterSet

The `ShaderParameterSet` manages a collection of parameters:
- Automatically handles parameter updates
- Calculates total push constant size
- Provides iteration over all parameters

### VulkanRenderer Integration

The renderer dynamically pushes constants based on parameter types:
1. Base color (vec4) - always pushed first
2. Custom parameters - pushed in order with proper alignment
3. Fallback to legacy `getShaderData()` if no custom params exist

### Memory Layout

Push constants are aligned to vec4 boundaries:
- `float` → 4 bytes
- `vec2` → 8 bytes
- `vec3` → 16 bytes (padded to vec4)
- `vec4` → 16 bytes

## Example: Custom Shader Implementation

### Material Setup (C++)

```cpp
Material waterMaterial("water", glm::vec4(0.2f, 0.4f, 0.8f, 0.7f));
waterMaterial.setParamVec2("waveSpeed", glm::vec2(0.5f, 0.3f));
waterMaterial.setParamScalar("waveHeight", 0.15f);
waterMaterial.setParamVec3("deepColor", glm::vec3(0.0f, 0.1f, 0.4f));
waterMaterial.setParamScalar("fresnelPower", 3.0f);
```

### Shader Usage (GLSL)

```glsl
// Fragment shader
layout(push_constant) uniform PushConstants {
    mat4 model;           // Offset 0
    vec4 baseColor;       // Offset 64
    vec2 waveSpeed;       // Offset 80
    float waveHeight;     // Offset 88
    vec4 deepColor;       // Offset 96 (vec3 padded to vec4)
    float fresnelPower;   // Offset 112
} pushConstants;

void main() {
    vec3 color = mix(
        pushConstants.baseColor.rgb,
        pushConstants.deepColor.rgb,
        depth
    );
    
    // Apply wave effect
    float wave = sin(time * pushConstants.waveSpeed.x) * pushConstants.waveHeight;
    
    // Apply fresnel
    float fresnel = pow(1.0 - NdotV, pushConstants.fresnelPower);
    
    fragColor = vec4(color, pushConstants.baseColor.a);
}
```

## Migration from Legacy System

The new system is backward compatible. Old code using `setCustomParam()` will automatically:
1. Store in the legacy map
2. Add to the new parameter system as a vec4

```cpp
// Old way (still works)
material.setCustomParam("color2", glm::vec4(1.0f, 0.0f, 0.0f, 1.0f));

// New way (recommended)
material.setParamVec4("color2", glm::vec4(1.0f, 0.0f, 0.0f, 1.0f));
```

## Best Practices

1. **Use specific types**: Instead of packing data into vec4, use appropriate types (scalar, vec2, vec3)
2. **Name consistently**: Use clear, descriptive parameter names
3. **Document shader requirements**: Keep shader parameter lists documented
4. **Consider push constant limits**: Vulkan has limits on push constant sizes (typically 128-256 bytes)
5. **Group related parameters**: Keep related parameters together for better cache coherency

## Limitations

- Push constants have size limits (check `maxPushConstantsSize` in device properties)
- Textures still require descriptor sets (not handled by push constants)
- Parameter order matters for shader layout matching
- Vec3 parameters are padded to vec4 size for alignment

## Future Enhancements

Potential improvements:
- Automatic shader parameter reflection
- Material templates with predefined parameter layouts
- Runtime parameter validation against shader requirements
- Support for material property blocks (uniform buffers)
