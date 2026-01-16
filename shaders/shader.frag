// Vulkan GLSL Fragment Shader — Forward PBR (metallic/roughness)
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

layout(location = 0) in vec3 inNormal;      // world-space normal
layout(location = 1) in vec2 inTexCoord;
layout(location = 2) in vec3 inWorldPos;
layout(location = 3) in vec3 inViewPos;     // camera position (world)
layout(location = 4) in vec3 inLightDir;    // direction TO light (normalized)
layout(location = 5) in vec3 inLightColor;  // radiance
layout(location = 6) in float inFog;
layout(location = 7) in vec4 inFogColor;    // scene fog color
layout(location = 8) in flat uint inInstanceIndex; // instance index for indirect rendering

layout(location = 0) out vec4 outFragColor;

// ─────────────────────────────────────────────
// Material textures (PBR metallic/roughness)
layout(set = 0, binding = 1) uniform sampler2D u_baseColorTex;
layout(set = 0, binding = 2) uniform sampler2D u_metallicRoughnessTex; // B=metallic, G=roughness
layout(set = 0, binding = 3) uniform sampler2D u_emissiveTex;

layout(binding = 5) readonly buffer InstanceBuffer {
    IndirectInstanceData instances[];
} instanceBuffer;

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
    IndirectInstanceData instance = instanceBuffer.instances[inInstanceIndex];

    // ─────────────────────────────────────────
    // Material inputs
    vec4 baseColor = texture(u_baseColorTex, inTexCoord) * instance.baseColorFactor;

    // Use material properties from instance data
    float metallic = instance.metallic;
    float roughness = clamp(instance.roughness, 0.04, 1.0);

    vec3 emissive = texture(u_emissiveTex, inTexCoord).rgb * instance.emissiveFactor;

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
    color = mix(color, inFogColor.rgb, inFog);

    outFragColor = vec4(color, baseColor.a);
}
