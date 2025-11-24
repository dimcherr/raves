@ctype mat4 glm::mat4
@ctype mat3 glm::mat3
@ctype vec2 glm::vec2
@ctype vec3 glm::vec3
@ctype vec4 glm::vec4



@vs postprocess_vs

layout (binding = 0) uniform postprocess_vs_params {
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



@fs postprocess_fs

const float PI = 3.14159265358979323846;

layout (binding = 1) uniform postprocess_fs_params {
    vec4 tint;
    float time;
    float intensity;
};

layout (binding = 0) uniform sampler ppSmp;
layout (binding = 1) uniform sampler ppNoiseSmp;
layout (binding = 0) uniform texture2D ppTexture;
layout (binding = 1) uniform texture2D ppNoiseTex;

in vec2 uv;

out vec4 fragColor;

//float hash(vec2 _v) {
    //return fract(sin(dot(_v, vec2(89.44, 19.36))) * 22189.22);
//}

//float iHash(vec2 _v, vec2 _r) {
    //float h00 = hash(vec2(floor(_v * _r + vec2(0.0, 0.0)) / _r));
    //float h10 = hash(vec2(floor(_v * _r + vec2(1.0, 0.0)) / _r));
    //float h01 = hash(vec2(floor(_v * _r + vec2(0.0, 1.0)) / _r));
    //float h11 = hash(vec2(floor(_v * _r + vec2(1.0, 1.0)) / _r));
    //vec2 ip = vec2(smoothstep(vec2(0.0, 0.0), vec2(1.0, 1.0), mod(_v*_r, 1.0)));
    //return (h00 * (1.0 - ip.x) + h10 * ip.x) * (1.0 - ip.y) + (h01 * (1.0 - ip.x) + h11 * ip.x) * ip.y;
//}

//float noise(vec2 _v) {
    //float sum = 0.0;
    //for(int i = 1; i < 9; i++) {
        //sum += iHash(_v + vec2(i), vec2(2.0 * pow(2.0, float(i)))) / pow(2.0, float(i));
    //}
    //return sum;
//}

float noise(vec2 _v) {
    return texture(sampler2D(ppNoiseTex, ppNoiseSmp), _v).r;
}

void main() {
    vec2 uvn = uv;
    vec4 rawColor = texture(sampler2D(ppTexture, ppSmp), uv) * tint;

    uvn.x += (noise(vec2(uvn.y, time)) - 0.5) * 0.001;
    uvn.x += (noise(vec2(uvn.y * 100.0, time * 10.0)) - 0.5) * 0.002;
    //uvn.x = uvn.x;

    vec4 color = texture(sampler2D(ppTexture, ppSmp), uvn) * tint;
// bloom
    for(int x = 0; x < 4; x++) {
        float xx = x * 0.5;
        color.xyz += vec3(
            texture(sampler2D(ppTexture, ppSmp), uvn + vec2(xx - 0.5, 0.0) * 7E-3).x,
            texture(sampler2D(ppTexture, ppSmp), uvn + vec2(xx - 1.0, 0.0) * 7E-3).y,
            texture(sampler2D(ppTexture, ppSmp), uvn + vec2(xx - 1.5, 0.0) * 7E-3).z
        ) * 0.1;
    }

    //color *= 1.0 + clamp(noise(vec2(0.0, uv.y + time * 0.2)) * 0.6 - 0.25, 0.0, 0.1) * 0.25;
    fragColor = mix(rawColor, color, intensity);
    //fragColor = texture(sampler2D(ppTexture, ppSmp), uv) * tint;
}

@end



@program postprocess postprocess_vs postprocess_fs
