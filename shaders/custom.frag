#version 450

layout(location = 0) in vec3 fragColor;
layout(location = 1) in vec2 fragTexCoord;
layout(location = 2) in vec4 fragBaseColor;
layout(location = 3) in vec3 fragNormal;
layout(location = 4) in vec3 fragWorldPos;
layout(location = 5) in vec4 fragCustomData;
layout(location = 6) in float fragTime;

layout(location = 0) out vec4 outColor;

void main() {
    // Extract the two colors from customData
    // We'll pack 2 vec3 colors as: color1.rg in xy, color1.b + color2.r in zw
    // For simplicity, let's use baseColor as color1 and customData as color2
    vec3 color1 = fragBaseColor.rgb;
    vec3 color2 = fragCustomData.rgb;
    
    // Oscillate between 0 and 1 every second
    float t = fract(fragTime);
    
    // Smooth interpolation (similar to slerp smoothness)
    t = smoothstep(0.0, 1.0, t);
    
    // Interpolate between colors
    vec3 interpolatedColor = mix(color1, color2, t);
    
    // Simple lighting
    vec3 normal = normalize(fragNormal);
    vec3 lightDir = normalize(vec3(0.5, -1.0, 0.3));
    float diffuse = max(dot(normal, -lightDir), 0.0) * 0.8 + 0.2;
    
    outColor = vec4(interpolatedColor * diffuse, fragBaseColor.a);
}
