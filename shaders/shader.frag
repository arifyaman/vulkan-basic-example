#version 450

layout(binding = 1) uniform sampler2D texSampler;

layout(location = 0) in vec3 fragColor;
layout(location = 1) in vec2 fragTexCoord;
layout(location = 2) in vec4 fragBaseColor;
layout(location = 3) in vec3 fragNormal;
layout(location = 4) in vec3 fragLightDir;

layout(location = 0) out vec4 outColor;

void main() {
    // Directional light from scene manager
    vec3 lightDir = normalize(fragLightDir);
    vec3 normal = normalize(fragNormal);
    
    // Ambient + diffuse lighting
    float ambient = 0.3;
    float diffuse = max(dot(normal, -lightDir), 0.0);
    float lighting = ambient + diffuse * 0.7;
    
    // Apply lighting to base color
    outColor = vec4(fragBaseColor.rgb * lighting, fragBaseColor.a);
}