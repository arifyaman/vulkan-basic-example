#version 450

layout(push_constant) uniform PushConstants {
    mat4 model;
} pc;

layout(binding = 0) uniform UniformBufferObject {
    mat4 view;
    mat4 proj;
    vec3 cameraPos;
    vec3 lightDir;     // direction TO light (normalized)
    vec3 lightColor;   // light radiance
    vec4 fogColor;     // scene-wide fog color
    float fogDensity;  // fog density factor
} ubo;

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

void main() {
    vec4 worldPos = pc.model * vec4(inPosition, 1.0);
    gl_Position = ubo.proj * ubo.view * worldPos;

    // Transform normal to world space (assuming uniform scaling)
    outNormal = normalize(mat3(pc.model) * inNormal);
    outTexCoord = inTexCoord;
    outWorldPos = worldPos.xyz;
    outViewPos = ubo.cameraPos;
    outLightDir = ubo.lightDir;
    outLightColor = ubo.lightColor;
    outFogColor = ubo.fogColor;

    // Simple fog based on distance
    float distance = length(ubo.cameraPos - worldPos.xyz);
    outFog = clamp(distance * ubo.fogDensity, 0.0, 1.0);
}
