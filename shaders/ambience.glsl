@ctype mat4 glm::mat4
@ctype mat3 glm::mat3
@ctype vec2 glm::vec2
@ctype vec3 glm::vec3
@ctype vec4 glm::vec4

@vs ambience_vs

layout (binding = 0) uniform ambience_vs_params {
    mat4 mvp;
};

layout (location = 0) in vec3 vertexPos;
layout (location = 1) in vec2 vertexUV;

out vec2 uv;

void main() {
    gl_Position = mvp * vec4(vertexPos, 1.0);
    uv = vertexUV;
}

@end

@fs ambience_fs

const float PI = 3.14159265358979323846;

layout (binding = 1) uniform ambience_fs_params {
    float ambientFactor;
    float time;
    float yaw;
};

layout (binding = 0) uniform sampler ambienceSampler;
layout (binding = 0) uniform texture2D ambienceAlbedoTex;
layout (binding = 1) uniform texture2D ambienceLightingTex;
layout (binding = 2) uniform texture2D ambiencePosTex;

in vec2 uv;

out vec4 fragColor;

float LinearizeDepth(float depth) {
    float near = 0.1; 
    float far  = 100.0; 
    float z = depth * 2.0 - 1.0; // back to NDC 
    float linear = (2.0 * near * far) / (far + near - z * (far - near)) + 0.0001;

    float normScreenX = gl_FragCoord.x / 480.0;
    float distToCenter = abs(normScreenX - 0.5) * PI; // 0.0 - PI 
    float s = (1.0 - (sin(distToCenter + sin(time + yaw) * 0.2) + 1.0) / 2.0);

    float fog = clamp((linear - 10.0 * s) / 5.0, 0.0, 1.0);

    return clamp(fog, 0.0, 1.0);
}

void main() {
    vec4 pos = texture(sampler2D(ambiencePosTex, ambienceSampler), uv);
    vec4 albedo = texture(sampler2D(ambienceAlbedoTex, ambienceSampler), uv);
    vec3 ambient = albedo.rgb * ambientFactor;
    vec3 lighting = texture(sampler2D(ambienceLightingTex, ambienceSampler), uv).rgb;
    vec3 color = ambient + lighting;
    vec3 emissive = albedo.rgb * albedo.a;

    // HDR tonemapping
    color = pow(color, color + vec3(1.0));
    
    // Gamma correction
    color = pow(color, vec3(0.4545));

    fragColor = vec4(color + emissive, 1.0 - floor(pos.a));
}

@end

@program ambience ambience_vs ambience_fs
