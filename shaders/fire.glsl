@ctype mat4 glm::mat4
@ctype mat3 glm::mat3
@ctype vec2 glm::vec2
@ctype vec3 glm::vec3
@ctype vec4 glm::vec4



@vs fire_vs

layout (binding = 0) uniform fire_vs_params {
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



@fs fire_fs

const float PI = 3.14159265358979323846;

layout (binding = 1) uniform fire_fs_params {
    float time;
};

layout (binding = 0) uniform sampler fireSmp;

layout (binding = 0) uniform texture2D fireCloudTex;
layout (binding = 1) uniform texture2D fireGradientTex;

in vec2 uv;

out vec4 fragColor;

void main() {
    vec4 cloud = texture(sampler2D(fireCloudTex, fireSmp), vec2(uv.x, uv.y + time * 0.3));
    vec4 gradient = texture(sampler2D(fireGradientTex, fireSmp), vec2(uv.x, uv.y));

    float x = gradient.r;
    float y = cloud.r;

    float a = step(y, x);
    float b = step(y, x - 0.1);
    float c = a - b;
    float d = step(y, x - 0.3);
    float e = b - c;

    vec3 heartColor = vec3(1.0, 0.941, 0.05);
    vec3 midColor = vec3(1.0, 0.6, 0.031);
    vec3 rimColor = vec3(1.0, 0.05, 0.0);

    vec3 colorA = mix(heartColor, rimColor, c);
    vec3 colorB = mix(colorA, midColor, e);

    fragColor = vec4(colorB, a);
}

@end



@program fire fire_vs fire_fs
