@ctype mat4 glm::mat4
@ctype mat3 glm::mat3
@ctype vec2 glm::vec2
@ctype vec3 glm::vec3
@ctype vec4 glm::vec4



@vs text_vs

layout (binding = 0) uniform text_vs_params {
    mat4 mvp;
};

layout (location = 0) in vec3 vertexPos;
layout (location = 1) in vec2 vertexUV;
layout (location = 2) in vec4 instPos;
layout (location = 3) in vec4 instUV;
layout (location = 4) in vec4 instDeform;
layout (location = 5) in vec4 instColor;
layout (location = 6) in vec4 instMVP0;
layout (location = 7) in vec4 instMVP1;
layout (location = 8) in vec4 instMVP2;
layout (location = 9) in vec4 instMVP3;

out vec2 uv;
out vec4 outInstColor;

void main() {
    mat4 instMVP = mat4(instMVP0, instMVP1, instMVP2, instMVP3);
    vec4 p = vec4(vertexPos * vec3(instPos.zw, 1.0) * vec3(instDeform.zw, 1.0) + vec3(instPos.xy, 0.0) + vec3(instDeform.xy, 0.0), 1.0);
    gl_Position = instMVP * mvp * p;
    uv = vertexUV * vec2(instUV.z, instUV.w) + vec2(instUV.x, instUV.y);
    outInstColor = instColor;
}
@end



@fs text_fs

layout (binding = 1) uniform text_fs_params {
    vec4 tint;
};

layout (binding = 0) uniform sampler textSmp;
layout (binding = 0) uniform texture2D fontAtlas;

in vec2 uv;
in vec4 outInstColor;

out vec4 fragColor;

void main() {
    float alpha = texture(sampler2D(fontAtlas, textSmp), uv).r;
    fragColor = vec4(1.0, 1.0, 1.0, alpha) * tint * outInstColor;
}

@end



@program text text_vs text_fs
