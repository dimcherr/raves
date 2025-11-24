@ctype mat4 glm::mat4
@ctype mat3 glm::mat3
@ctype vec2 glm::vec2
@ctype vec3 glm::vec3
@ctype vec4 glm::vec4

@vs draw_vs

layout (binding = 0) uniform draw_vs_params {
    mat4 mvp;
};

layout (location = 0) in vec3 vertexPos;
layout (location = 1) in vec2 vertexUV;

out vec2 uv;

void main() {
    gl_Position = mvp * vec4(vertexPos, 1.f);
    uv = vertexUV;
}

@end



@fs draw_fs

layout (binding = 1) uniform draw_fs_params {
    vec4 color;
};

in vec2 uv;

out vec4 fragColor;

void main() {
    fragColor = color;
}

@end



@program draw draw_vs draw_fs
