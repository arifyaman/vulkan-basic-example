#version 450

layout(push_constant) uniform PushConstants {
    mat4 model;
    vec4 baseColor;
    vec4 specularData; // rgb = specularColor, a = shininess
} pc;

layout(binding = 0) uniform UniformBufferObject {
    mat4 view;
    mat4 proj;
    vec4 directionalLight; // xyz = direction, w = intensity
    vec3 cameraPos;
} ubo;

layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec3 inColor;
layout(location = 2) in vec2 inTexCoord;
layout(location = 3) in vec3 inNormal;

layout(location = 0) out vec3 fragColor;
layout(location = 1) out vec2 fragTexCoord;
layout(location = 2) out vec4 fragBaseColor;
layout(location = 3) out vec3 fragNormal;
layout(location = 4) out vec4 fragDirectionalLight;
layout(location = 5) out vec3 fragWorldPos;
layout(location = 6) out vec3 fragCameraPos;
layout(location = 7) out vec4 fragSpecularData;

void main() {
    vec4 worldPos = pc.model * vec4(inPosition, 1.0);
    gl_Position = ubo.proj * ubo.view * worldPos;
    fragColor = inColor;
    fragTexCoord = inTexCoord;
    fragBaseColor = pc.baseColor;
    // Transform normal to world space (assuming uniform scaling)
    fragNormal = mat3(pc.model) * inNormal;
    fragDirectionalLight = ubo.directionalLight;
    fragWorldPos = worldPos.xyz;
    fragCameraPos = ubo.cameraPos;
    fragSpecularData = pc.specularData;
}
