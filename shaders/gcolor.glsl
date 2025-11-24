@ctype mat4 glm::mat4
@ctype mat3 glm::mat3
@ctype vec2 glm::vec2
@ctype vec3 glm::vec3
@ctype vec4 glm::vec4

@vs gcolor_vs

layout (binding = 0) uniform gcolor_vs_params {
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

void main() {
    pos = vec3(matModel * vec4(vertexPos, 1.0));
    uv = vertexUV;

    vec3 T = normalize(vec3(matNormal * vec4(vertexTangent.xyz, 1.0)));
    vec3 N = normalize(vec3(matNormal * vec4(vertexNormal, 1.0)));
    //vec3 B = cross(N, T) * vertexTangent.w;
    vec3 B = cross(N, T);

    //T = normalize(T - dot(T, N) * N);
    TBN = mat3(T, B, N);
    //vertexN = N;

    gl_Position = mvp * vec4(vertexPos, 1.0);
}

@end

@fs gcolor_fs

layout(binding = 0) uniform sampler gcolorSampler;
layout(binding = 0) uniform texture2D gcolorORMTex;
layout(binding = 1) uniform texture2D gcolorAlbedoTex;
layout(binding = 2) uniform texture2D gcolorEmissiveTex;
layout(binding = 3) uniform texture2D gcolorNormalTex;

layout(binding = 1) uniform gcolor_fs_params {
    vec4 albedoTint;
    vec2 uvTiling;
    vec2 uvOffset;
    float metallicFactor;
    float roughnessFactor;
    float emissiveFactor;
};

in vec3 pos;
in vec2 uv;
in mat3 TBN;

out vec4 outORM;
out vec4 outAlbedo;
out vec4 outPosition;
out vec4 outWorldNormal;

void main() {
    vec2 texCoord = vec2(uv.x * uvTiling.x + uvOffset.x, uv.y * uvTiling.y + uvOffset.y);

    vec4 orm = texture(sampler2D(gcolorORMTex, gcolorSampler), texCoord);
    float ao = clamp(orm.r, 0.0, 1.0);
    float roughness = clamp(orm.g * roughnessFactor, 0.04, 1.0);
    float metallic = clamp(orm.b * metallicFactor, 0.04, 1.0);
    outORM = vec4(ao, roughness, metallic, 1.0);

    vec4 tintedAlbedo = albedoTint * texture(sampler2D(gcolorAlbedoTex, gcolorSampler), texCoord);
    vec4 emissive = emissiveFactor * texture(sampler2D(gcolorEmissiveTex, gcolorSampler), texCoord);
    outAlbedo = vec4(tintedAlbedo.rgb, emissive.r);

    outPosition = vec4(pos, gl_FragCoord.z);

    vec3 N = texture(sampler2D(gcolorNormalTex, gcolorSampler), texCoord).rgb;
    N = N * 2.0 - 1.0;
    N = normalize(TBN * N);
    outWorldNormal = vec4(N, 1.0);
}

@end

@program gcolor gcolor_vs gcolor_fs
