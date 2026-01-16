#version 450

struct IndirectInstanceData
{
    mat4 modelMatrix;
    vec4 baseColorFactor;
    vec3 emissiveFactor;
    float metallic;
    float roughness;
    uint baseColorTextureIndex;
    uint metallicRoughnessTextureIndex;
    uint emissiveTextureIndex;
    uint vertexBufferIndex;
    uint indexBufferIndex;
    uint firstIndex;
    uint indexCount;
    uint vertexOffset;
};

layout(binding = 0) uniform UniformBufferObject {
    mat4 view;
    mat4 proj;
    vec3 cameraPos;
    vec3 lightDir;     // direction TO light (normalized)
    vec3 lightColor;   // light radiance
    vec4 fogColor;     // scene-wide fog color
    float fogDensity;  // fog density factor
    float fogStart;    // minimum distance before fog starts
} ubo;

layout(binding = 5) readonly buffer InstanceBuffer {
    IndirectInstanceData instances[];
} instanceBuffer;

layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec3 inColor;
layout(location = 2) in vec2 inTexCoord;
layout(location = 3) in vec3 inNormal;

layout(location = 0) out vec3 outNormal;      // world-space normal
layout(location = 1) out vec2 outTexCoord;
layout(location = 2) out vec3 outWorldPos;
layout(location = 3) out vec3 outViewPos;     // camera position (world)
layout(location = 4) out vec3 outLightDir;    // direction TO light (normalized)
layout(location = 5) out vec3 outLightColor;  // radiance
layout(location = 6) out float outFog;
layout(location = 7) out vec4 outFogColor;    // scene fog color
layout(location = 8) out flat uint outInstanceIndex; // instance index for indirect rendering

void main() {
    IndirectInstanceData instance = instanceBuffer.instances[gl_InstanceIndex];

    vec4 worldPos = instance.modelMatrix * vec4(inPosition, 1.0);
    gl_Position = ubo.proj * ubo.view * worldPos;

    // Transform normal to world space (assuming uniform scaling)
    outNormal = normalize(mat3(instance.modelMatrix) * inNormal);
    outTexCoord = inTexCoord;
    outWorldPos = worldPos.xyz;
    outViewPos = ubo.cameraPos;
    outLightDir = ubo.lightDir;
    outLightColor = ubo.lightColor;
    outFogColor = ubo.fogColor;

    // Exponential fog with minimum distance
    float distance = length(ubo.cameraPos - worldPos.xyz);
    float fogDistance = max(distance - ubo.fogStart, 0.0);
    outFog = 1.0 - exp(-fogDistance * ubo.fogDensity);

    // Pass instance index to fragment shader
    outInstanceIndex = gl_InstanceIndex;
}
