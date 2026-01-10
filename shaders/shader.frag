#version 450

layout(binding = 1) uniform sampler2D texSampler;

layout(location = 0) in vec3 fragColor;
layout(location = 1) in vec2 fragTexCoord;
layout(location = 2) in vec4 fragBaseColor;
layout(location = 3) in vec3 fragNormal;
layout(location = 4) in vec4 fragDirectionalLight;
layout(location = 5) in vec3 fragWorldPos;
layout(location = 6) in vec3 fragCameraPos;
layout(location = 7) in vec4 fragSpecularData;

layout(location = 0) out vec4 outColor;

void main() {
    // Directional light from scene manager (xyz = direction, w = intensity)
    vec3 lightDir = normalize(fragDirectionalLight.xyz);
    float lightIntensity = fragDirectionalLight.w;
    vec3 normal = normalize(fragNormal);
    vec3 viewDir = normalize(fragCameraPos - fragWorldPos);
    
    // Extract material properties
    vec3 specularColor = fragSpecularData.rgb;
    float shininess = fragSpecularData.a;
    
    // Ambient lighting (constant background illumination)
    float ambient = 0.2;
    
    // Diffuse lighting (scaled by directional light intensity)
    float diffuse = max(dot(normal, -lightDir), 0.0) * lightIntensity;
    
    // Specular lighting (scaled by directional light intensity and material specular color)
    vec3 halfDir = normalize(-lightDir + viewDir);
    float specularStrength = pow(max(dot(normal, halfDir), 0.0), shininess) * lightIntensity;
    vec3 specular = specularStrength * specularColor;
    
    // Combine lighting
    vec3 diffuseContrib = fragBaseColor.rgb * (ambient + diffuse * 0.6);
    vec3 finalColor = diffuseContrib + specular * 0.4;
    
    // Apply to output
    outColor = vec4(finalColor, fragBaseColor.a);
}