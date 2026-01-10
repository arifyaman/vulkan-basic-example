#version 450

layout(binding = 1) uniform sampler2D texSampler;

layout(location = 0) in vec3 fragColor;
layout(location = 1) in vec2 fragTexCoord;
layout(location = 2) in vec4 fragBaseColor;
layout(location = 3) in vec3 fragNormal;
layout(location = 4) in vec3 fragLightDir;
layout(location = 5) in vec3 fragWorldPos;
layout(location = 6) in vec3 fragCameraPos;

layout(location = 0) out vec4 outColor;

void main() {
    // Directional light from scene manager
    vec3 lightDir = normalize(fragLightDir);
    vec3 normal = normalize(fragNormal);
    vec3 viewDir = normalize(fragCameraPos - fragWorldPos);
    
    // Ambient lighting
    float ambient = 0.3;
    
    // Diffuse lighting
    float diffuse = max(dot(normal, -lightDir), 0.0);
    
    // Specular lighting (Blinn-Phong)
    vec3 halfDir = normalize(-lightDir + viewDir);
    float specular = pow(max(dot(normal, halfDir), 0.0), 32.0);
    
    // Combine lighting
    float lighting = ambient + diffuse * 0.6 + specular * 0.4;
    
    // Apply lighting to base color
    outColor = vec4(fragBaseColor.rgb * lighting, fragBaseColor.a);
}