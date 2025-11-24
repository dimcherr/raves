@ctype mat4 glm::mat4
@ctype mat3 glm::mat3
@ctype vec2 glm::vec2
@ctype vec3 glm::vec3
@ctype vec4 glm::vec4


@vs shadow_vs
@glsl_options fixup_clipspace

layout (binding = 0) uniform shadow_vs_params {
    mat4 mvp;
};

layout (location = 0) in vec3 vertexPos;

void main() {
    gl_Position = mvp * vec4(vertexPos, 1.0);
}

@end

@fs shadow_fs

const float PI = 3.14159265358979323846;

layout (binding = 1) uniform shadow_fs_params {
    float dummy;
};

out vec4 fragColor;

void main() {
    fragColor = vec4(dummy);
}

@end

@program shadow shadow_vs shadow_fs
