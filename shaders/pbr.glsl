@ctype mat4 glm::mat4
@ctype mat3 glm::mat3
@ctype vec2 glm::vec2
@ctype vec3 glm::vec3
@ctype vec4 glm::vec4



@vs pbr_vs

layout (binding = 0) uniform pbr_vs_params {
    mat4 mvp;
    mat4 matModel;
    mat4 matNormal;
};

layout (location = 0) in vec3 vertexPos;
layout (location = 1) in vec2 vertexUV;
layout (location = 2) in vec3 vertexNormal;
layout (location = 3) in vec4 vertexTangent;

out vec3 pos;
out vec2 uv;
out mat3 TBN;
out vec3 vertexN;

void main() {
    pos = vec3(matModel * vec4(vertexPos, 1.0));
    uv = vertexUV;

    vec3 N = normalize(vec3(matNormal * vec4(vertexNormal, 0.0)));
    vec3 T = normalize(vec3(matNormal * vec4(vertexTangent.xyz, 0.0)));
    T = normalize(T - dot(T, N) * N);
    vec3 B = cross(N, T) * vertexTangent.w;
    TBN = mat3(T, B, N);
    vertexN = N;

    gl_Position = mvp * vec4(vertexPos, 1.0);
}

@end



@fs pbr_fs

const float PI = 3.14159265358979323846;
const int MAX_POINT_LIGHTS = 8;

layout(binding = 1) uniform pbr_fs_params {
    vec3 viewPos;
    vec3 ambientColor;
    vec3 tint;
    vec3 emissiveTint;
    vec3 highlight;
    vec2 tiling;
    vec2 offset;
    float metallicFactor;
    float roughnessFactor;
    float emissiveFactor;
    float ambientFactor;
    int pointLightCount;
    vec4 pointLightPos[MAX_POINT_LIGHTS];
    vec4 pointLightColor[MAX_POINT_LIGHTS];
};

layout(binding = 0) uniform sampler smp;

layout(binding = 0) uniform texture2D albedoMap;
layout(binding = 1) uniform texture2D normalMap;
layout(binding = 2) uniform texture2D ormMap;
layout(binding = 3) uniform texture2D emissiveMap;

in vec3 pos;
in vec2 uv;
in mat3 TBN;
in vec3 vertexN;

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
    vec2 uv = vec2(uv.x * tiling.x + offset.x, uv.y * tiling.y + offset.y);
    vec3 albedo = texture(sampler2D(albedoMap, smp), uv).rgb;
    albedo = tint * albedo;

    vec4 orm = texture(sampler2D(ormMap, smp), uv);

    float ao = clamp(orm.r, 0.0, 1.0);
    float roughness = clamp(orm.g * roughnessFactor, 0.04, 1.0);
    float metallic = clamp(orm.b * metallicFactor, 0.04, 1.0);

    vec3 emissive = texture(sampler2D(emissiveMap, smp), uv).rgb;

    vec3 N = texture(sampler2D(normalMap, smp), uv).rgb;
    N = N * 2.0 - 1.0;
    N = normalize(TBN * N);

    vec3 V = normalize(viewPos - pos);

    vec3 baseRefl = mix(vec3(0.04), albedo, metallic);
    vec3 lightAccum = vec3(0.0);  // Acumulate lighting lum

    for (int i = 0; i < pointLightCount; i++) {
        vec3 L = normalize(pointLightPos[i].xyz - pos);      // Compute light vector
        vec3 H = normalize(V + L);                                  // Compute halfway bisecting vector
        float dist = length(pointLightPos[i].xyz - pos);     // Compute distance to light

        float attenuation = pointLightPos[i].a / (dist * dist * 0.27);                   // Compute attenuation
        vec3 radiance = pointLightColor[i].rgb * pointLightColor[i].a * attenuation; // Compute input radiance, light energy comming in

        float nDotV = max(dot(N, V), 0.0000001);
        float nDotL = max(dot(N, L), 0.0000001);
        float hDotV = max(dot(H, V), 0.0);
        float nDotH = max(dot(N, H), 0.0);
        float D = GgxDistribution(nDotH, roughness);    // Larger the more micro-facets aligned to H
        float G = GeomSmith(nDotV, nDotL, roughness);   // Smaller the more micro-facets shadow
        vec3 F = SchlickFresnel(hDotV, baseRefl);       // Fresnel proportion of specular reflectance

        vec3 spec = (D * G * F) / (4.0 * nDotV * nDotL);
        
        vec3 kD = vec3(1.0) - F;
        
        kD *= 1.0 - metallic;
        lightAccum += (kD * albedo / PI + spec) * radiance * nDotL * ceil(max(dot(vertexN, L), 0.0)); // Angle of light has impact on result
    }
    
    vec3 ambientFinal = (ambientColor + albedo) * ambientFactor * 0.5;
    return ambientFinal + lightAccum * ao + emissive * emissiveFactor * emissiveTint + highlight;
}

void main() {
    vec3 color = ComputePBR();

     // HDR tonemapping
    color = pow(color, color + vec3(1.0));
    
    // Gamma correction
    color = pow(color, vec3(0.4545));

    fragColor = vec4(color, 1.0);
}

@end



@program pbr pbr_vs pbr_fs
