#version 450

layout(push_constant) uniform PushConstants {
    mat4 model;
    vec4 baseColor;
    vec4 customData; // For custom shader: xy = color1, zw = color2 (packed)
} pc;

layout(binding = 0) uniform UniformBufferObject {
    mat4 view;
    mat4 proj;
    vec4 directionalLight;
    vec3 cameraPos;
    float time; // Time uniform for animation
} ubo;

layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec3 inColor;
layout(location = 2) in vec2 inTexCoord;
layout(location = 3) in vec3 inNormal;

layout(location = 0) out vec3 fragColor;
layout(location = 1) out vec2 fragTexCoord;
layout(location = 2) out vec4 fragBaseColor;
layout(location = 3) out vec3 fragNormal;
layout(location = 4) out vec3 fragWorldPos;
layout(location = 5) out vec4 fragCustomData;
layout(location = 6) out float fragTime;

void main() {
    vec4 worldPos = pc.model * vec4(inPosition, 1.0);
    gl_Position = ubo.proj * ubo.view * worldPos;
    fragColor = inColor;
    fragTexCoord = inTexCoord;
    fragBaseColor = pc.baseColor;
    fragNormal = mat3(pc.model) * inNormal;
    fragWorldPos = worldPos.xyz;
    fragCustomData = pc.customData;
    fragTime = ubo.time;
}
