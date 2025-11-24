@ctype mat4 glm::mat4
@ctype mat3 glm::mat3
@ctype vec2 glm::vec2
@ctype vec3 glm::vec3
@ctype vec4 glm::vec4

@block skin_utils
void skinned_pos_nrm(in vec4 pos, in vec4 nrm, in vec4 tang, in vec4 skin_weights, in uvec4 skin_indices, out vec4 skin_pos, out vec4 skin_nrm, out vec4 skin_tang) {
    skin_pos = vec4(0.0, 0.0, 0.0, 1.0);
    skin_nrm = vec4(0.0, 0.0, 0.0, 0.0);
    skin_tang = vec4(0.0, 0.0, 0.0, tang.w); // Preserve tangent.w (handedness)
    
    // Normalize weights to ensure they sum to 1
    float weightSum = dot(skin_weights, vec4(1.0));
    vec4 weights = (weightSum > 0.0) ? skin_weights / weightSum : vec4(0.0);
    
    ivec2 uv;
    vec4 xxxx, yyyy, zzzz;
    mat4 jointMat;
    
    // Process each joint influence
    for (int i = 0; i < 4; i++) {
        if (weights[i] > 0.0) {
            uv = ivec2(3 * skin_indices[i], gl_InstanceIndex);
            xxxx = texelFetch(sampler2D(pbrSkinnedJointTex, pbrSkinnedSmp), uv, 0);
            yyyy = texelFetch(sampler2D(pbrSkinnedJointTex, pbrSkinnedSmp), uv + ivec2(1,0), 0);
            zzzz = texelFetch(sampler2D(pbrSkinnedJointTex, pbrSkinnedSmp), uv + ivec2(2,0), 0);
            
            // Construct joint matrix
            jointMat = mat4(
                xxxx.x, yyyy.x, zzzz.x, 0.0,
                xxxx.y, yyyy.y, zzzz.y, 0.0,
                xxxx.z, yyyy.z, zzzz.z, 0.0,
                xxxx.w, yyyy.w, zzzz.w, 1.0
            );
            
            // Transform position (w = 1.0)
            skin_pos += jointMat * pos * weights[i];
            
            // Transform normal and tangent (w = 0.0 for vectors)
            skin_nrm.xyz += (jointMat * vec4(nrm.xyz, 0.0)).xyz * weights[i];
            skin_tang.xyz += (jointMat * vec4(tang.xyz, 0.0)).xyz * weights[i];
        }
    }
}
@end



@vs pbrSkinned_vs
layout (binding = 0) uniform pbrSkinned_vs_params {
    mat4 mvp;
    mat4 matModel;
    mat4 matNormal; // Should be inverse-transpose of matModel for correct normal transformation
};

@image_sample_type pbrSkinnedJointTex unfilterable_float
layout(binding=4) uniform texture2D pbrSkinnedJointTex;
@sampler_type pbrSkinnedSmp nonfiltering
layout(binding=4) uniform sampler pbrSkinnedSmp;

layout (location = 0) in vec3 vertexPos;
layout (location = 1) in vec2 vertexUV;
layout (location = 2) in vec3 vertexNormal;
layout (location = 3) in vec4 vertexTangent;
layout (location = 4) in uvec4 jindices;
layout (location = 5) in vec4 jweights;

out vec3 pos;
out vec2 uv;
out mat3 TBN;
out vec3 vertexN;

@include_block skin_utils

void main() {
    vec4 skinnedPos, skinnedNormal, skinnedTangent;
    skinned_pos_nrm(vec4(vertexPos, 1.0), vec4(vertexNormal, 0.0), vertexTangent, jweights, jindices, skinnedPos, skinnedNormal, skinnedTangent);

    pos = vec3(matModel * skinnedPos);
    uv = vertexUV;

    // Transform normal and tangent using matNormal (inverse-transpose of model matrix)
    vec3 N = normalize(vec3(matNormal * vec4(skinnedNormal.xyz, 0.0)));
    vec3 T = normalize(vec3(matNormal * vec4(skinnedTangent.xyz, 0.0)));
    
    // Orthogonalize T with respect to N
    T = normalize(T - dot(T, N) * N);
    
    // Compute bitangent with handedness
    vec3 B = normalize(cross(N, T) * skinnedTangent.w);
    
    // Construct TBN matrix
    TBN = mat3(T, B, N);
    vertexN = N;

    gl_Position = mvp * skinnedPos;
}
@end





@fs pbrSkinned_fs

const float PI = 3.14159265358979323846;
const int MAX_POINT_LIGHTS = 8;

layout(binding = 1) uniform pbrSkinned_fs_params {
    vec3 viewPos;
    vec3 ambientColor;
    vec3 tint;
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
    return ambientFinal + lightAccum * ao + emissive * emissiveFactor;
}

void main() {
    vec3 color = ComputePBR();

     // HDR tonemapping
    color = pow(color, color + vec3(1.0));
    
    // Gamma correction
    color = pow(color, vec3(0.4545));

    fragColor = vec4(color, 1.0) + vec4(highlight, 1.0);
}

@end



@program pbrSkinned pbrSkinned_vs pbrSkinned_fs
