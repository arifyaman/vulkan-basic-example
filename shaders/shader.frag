// Vulkan GLSL Fragment Shader — Forward PBR (metallic/roughness)
#version 450

layout(location = 0) in vec3 inNormal;      // world-space normal
layout(location = 1) in vec2 inTexCoord;
layout(location = 2) in vec3 inWorldPos;
layout(location = 3) in vec3 inViewPos;     // camera position (world)
layout(location = 4) in vec3 inLightDir;    // direction TO light (normalized)
layout(location = 5) in vec3 inLightColor;  // radiance
layout(location = 6) in float inFog;

layout(location = 0) out vec4 outFragColor;

// ─────────────────────────────────────────────
// Material textures (PBR metallic/roughness)
layout(set = 0, binding = 1) uniform sampler2D u_baseColorTex;
layout(set = 0, binding = 2) uniform sampler2D u_metallicRoughnessTex; // B=metallic, G=roughness
layout(set = 0, binding = 3) uniform sampler2D u_emissiveTex;

// Use push constant for material properties instead of uniform buffer
layout(push_constant) uniform PushConstants {
    layout(offset = 64) vec4 baseColorFactor;  // offset after mat4 model matrix
    layout(offset = 80) vec3 emissiveFactor;
    layout(offset = 96) vec4 fogColor;
    layout(offset = 112) float metallic;
    layout(offset = 116) float roughness;
} pc;

const float PI = 3.14159265359;

// ─────────────────────────────────────────────
// PBR helper functions (Cook–Torrance / GGX)
float DistributionGGX(vec3 N, vec3 H, float roughness)
{
    float a = roughness * roughness;
    float a2 = a * a;
    float NdotH = max(dot(N, H), 0.0);
    float NdotH2 = NdotH * NdotH;

    float denom = (NdotH2 * (a2 - 1.0) + 1.0);
    return a2 / (PI * denom * denom);
}

float GeometrySchlickGGX(float NdotV, float roughness)
{
    float r = roughness + 1.0;
    float k = (r * r) / 8.0;
    return NdotV / (NdotV * (1.0 - k) + k);
}

float GeometrySmith(vec3 N, vec3 V, vec3 L, float roughness)
{
    float NdotV = max(dot(N, V), 0.0);
    float NdotL = max(dot(N, L), 0.0);
    float ggx1 = GeometrySchlickGGX(NdotV, roughness);
    float ggx2 = GeometrySchlickGGX(NdotL, roughness);
    return ggx1 * ggx2;
}

vec3 FresnelSchlick(float cosTheta, vec3 F0)
{
    return F0 + (1.0 - F0) * pow(1.0 - cosTheta, 5.0);
}

void main()
{
    // ─────────────────────────────────────────
    // Material inputs
    vec4 baseColor = texture(u_baseColorTex, inTexCoord) * pc.baseColorFactor;

    // Use material properties from push constants
    float metallic = pc.metallic;
    float roughness = clamp(pc.roughness, 0.04, 1.0);

    vec3 emissive = texture(u_emissiveTex, inTexCoord).rgb * pc.emissiveFactor;

    // ─────────────────────────────────────────
    // Lighting vectors
    vec3 N = normalize(inNormal);
    vec3 V = normalize(inViewPos - inWorldPos);
    vec3 L = normalize(inLightDir);
    vec3 H = normalize(V + L);

    float NdotL = max(dot(N, L), 0.0);

    // ─────────────────────────────────────────
    // Fresnel reflectance at normal incidence
    vec3 F0 = mix(vec3(0.04), baseColor.rgb, metallic);

    // Cook–Torrance BRDF
    float D = DistributionGGX(N, H, roughness);
    float G = GeometrySmith(N, V, L, roughness);
    vec3  F = FresnelSchlick(max(dot(H, V), 0.0), F0);

    vec3 numerator = D * G * F;
    float denom = 4.0 * max(dot(N, V), 0.0) * NdotL + 0.0001;
    vec3 specular = numerator / denom;

    vec3 kS = F;
    vec3 kD = (vec3(1.0) - kS) * (1.0 - metallic);

    vec3 diffuse = kD * baseColor.rgb / PI;

    vec3 radiance = inLightColor;
    vec3 directLighting = (diffuse + specular) * radiance * NdotL;

    // Add simple ambient lighting to prevent completely black areas
    vec3 ambient = baseColor.rgb * 0.1; // 10% ambient light

    vec3 color = directLighting + ambient + emissive;

    // ─────────────────────────────────────────
    // Fog
    color = mix(color, pc.fogColor.rgb, inFog);

    outFragColor = vec4(color, baseColor.a);
}
