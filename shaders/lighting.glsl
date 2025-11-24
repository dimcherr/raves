@ctype mat4 glm::mat4
@ctype mat3 glm::mat3
@ctype vec2 glm::vec2
@ctype vec3 glm::vec3
@ctype vec4 glm::vec4


@vs lighting_vs

layout (binding = 0) uniform lighting_vs_params {
    mat4 mvp;
};

layout (location = 0) in vec3 vertexPos;
layout (location = 1) in vec2 vertexUV;

out vec2 uv;
out vec3 lightPos;

void main() {
    vec4 p = mvp * vec4(vertexPos, 1.0);
    lightPos = p.xyz;
    gl_Position = p;
    uv = vertexUV;
}

@end

@fs lighting_fs

const float PI = 3.14159265358979323846;

layout (binding = 1) uniform lighting_fs_params {
    vec3 viewPos;
    vec4 pointLightPos;
    vec4 pointLightColor;
    vec2 screenSize;
    mat4 lightMVP;
};

layout (binding = 0) uniform sampler lightingSampler;
layout (binding = 0) uniform texture2D lightingAlbedoTex;
layout (binding = 1) uniform texture2D lightingNormalTex;
layout (binding = 2) uniform texture2D lightingORMTex;
layout (binding = 3) uniform texture2D lightingPosTex;

in vec2 uv;
in vec3 lightPos;

out vec4 fragColor;

vec3 SchlickFresnel(float hDotV, vec3 refl) {
    return refl + (1.0 - refl) * pow(1.0 - hDotV, 5.0);
}

float GgxDistribution(float nDotH, float roughness) {
    float a = roughness * roughness * roughness * roughness;
    float d = nDotH * nDotH * (a - 1.0) + 1.0;
    d = PI * d * d;
    return a / max(d, 0.0000001);
}

float GeomSmith(float nDotV, float nDotL, float roughness) {
    float r = roughness + 1.0;
    float k = (r * r) / 8.0;
    float ik = 1.0 - k;
    float ggx1 = nDotV / (nDotV * ik + k);
    float ggx2 = nDotL / (nDotL * ik + k);
    return ggx1 * ggx2;
}

vec3 ComputePBR() {
    vec2 screenUV = gl_FragCoord.xy / screenSize;
    vec4 ormTex = texture(sampler2D(lightingORMTex, lightingSampler), screenUV);
    vec4 posTex = texture(sampler2D(lightingPosTex, lightingSampler), screenUV);
    vec4 normalTex = texture(sampler2D(lightingNormalTex, lightingSampler), screenUV);
    vec3 localPos = vec3(ormTex.w, posTex.w, normalTex.w);

    vec3 albedo = texture(sampler2D(lightingAlbedoTex, lightingSampler), screenUV).rgb;
    vec3 orm = ormTex.rgb;
    vec3 N = normalTex.rgb;
    vec3 pos = posTex.xyz;

    float ao = orm.r;
    float roughness = orm.g;
    float metallic = orm.b;

    vec3 V = normalize(viewPos - pos);      // Compute light vector
    vec3 L = normalize(pointLightPos.xyz - pos);      // Compute light vector
    vec3 H = normalize(V + L);                                  // Compute halfway bisecting vector

    vec3 lightColor = pointLightColor.rgb;
    float dist = max(length(pointLightPos.xyz - pos), 0.00001);     // Compute distance to light

    vec3 baseRefl = mix(vec3(0.04), albedo, metallic);


    float attenuation = pointLightColor.a / (dist * dist * 0.27);                   // Compute attenuation
    vec3 radiance = pointLightColor.rgb * attenuation; // Compute input radiance, light energy comming in

    float nDotV = clamp(dot(N, V), 0.0, 1.0);
    float nDotL = clamp(dot(N, L), 0.0, 1.0);
    float hDotV = clamp(dot(H, V), 0.0, 1.0);
    float nDotH = clamp(dot(N, H), 0.0, 1.0);

    float D = GgxDistribution(nDotH, roughness);    // Larger the more micro-facets aligned to H
    float G = GeomSmith(nDotV, nDotL, roughness);   // Smaller the more micro-facets shadow
    vec3 F = SchlickFresnel(hDotV, baseRefl);       // Fresnel proportion of specular reflectance

    vec3 spec = (D * G * F) / (4.0 * nDotV * nDotL);
    vec3 kD = vec3(1.0) - F;
    kD *= 1.0 - metallic;
    vec3 lightingFinal = (kD * albedo / PI + spec * 0.15) * radiance * nDotL; // Angle of light has impact on result

    return lightingFinal * ao;
}

void main() {
    fragColor = vec4(ComputePBR(), 1.0);
}

@end

@program lighting lighting_vs lighting_fs
