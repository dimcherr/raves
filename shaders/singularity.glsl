@ctype mat4 glm::mat4
@ctype mat3 glm::mat3
@ctype vec2 glm::vec2
@ctype vec3 glm::vec3
@ctype vec4 glm::vec4



@vs singularity_vs

layout (binding = 0) uniform singularity_vs_params {
    mat4 mvp;
};

layout (location = 0) in vec3 vertexPos;
layout (location = 1) in vec2 vertexUV;

out vec2 uv;

void main() {
    vec4 p = vec4(vertexPos, 1.0);
    gl_Position = mvp * p;

    uv = vertexUV;
}

@end



@fs singularity_fs

const float PI = 3.14159265358979323846;

layout (binding = 1) uniform singularity_fs_params {
    float time;
    float screenRatio;
    vec4 tint;
};

in vec2 uv;

out vec4 fragColor;

void main() {
    vec2 _uv = uv;
    vec2 p = _uv - 0.5;
    p.x *= screenRatio;

    //float z = sin(time) * 1.2 + 1.58; 
    float z = time; 
    float l = length(p);

    vec3 color = vec3(0.0);

    for (int i = 0; i < 3; i++) {
        vec2 q = _uv;
        z += 0.09;

        q += p / l * (sin(z * PI * 0.5) + 1.2) * abs(sin((l * 8.0 - z * 2.0) * PI * 0.5));

        vec2 m = mod(q + float(i) * 0.005, 1.0) - 0.5;
        float d = length(m);

        float glow = 0.015 / pow(d, 1.4); 
        color[i] += glow;
    }

    color *= vec3(0.3, 0.6, 1.8); 
    color = clamp(color / (l + 0.03), 0.0, 1.0);

    fragColor = vec4(color * 0.5, 1.0) * tint;
}

@end



@program singularity singularity_vs singularity_fs
