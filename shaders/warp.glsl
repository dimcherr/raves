@ctype mat4 glm::mat4
@ctype mat3 glm::mat3
@ctype vec2 glm::vec2
@ctype vec3 glm::vec3
@ctype vec4 glm::vec4



@vs warp_vs

layout (binding = 0) uniform warp_vs_params {
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



@fs warp_fs

const float PI = 3.14159265358979323846;

layout (binding = 1) uniform warp_fs_params {
    float time;
    float opacity;
    float width;
};

layout (binding = 0) uniform sampler warpSmp;
layout (binding = 0) uniform texture2D warpNoiseSmp;

in vec2 uv;

out vec4 fragColor;


float colormapRed(float x) {
    if (x < 0.0) {
        return 54.0 / 255.0;
    } else if (x < 20049.0 / 82979.0) {
        return (829.79 * x + 54.51) / 255.0;
    } else {
        return 1.0;
    }
}

float colormapGreen(float x) {
    if (x < 20049.0 / 82979.0) {
        return 0.0;
    } else if (x < 327013.0 / 810990.0) {
        return (8546482679670.0 / 10875673217.0 * x - 2064961390770.0 / 10875673217.0) / 255.0;
    } else if (x <= 1.0) {
        return (103806720.0 / 483977.0 * x + 19607415.0 / 483977.0) / 255.0;
    } else {
        return 1.0;
    }
}

float colormapBlue(float x) {
    if (x < 0.0) {
        return 54.0 / 255.0;
    } else if (x < 7249.0 / 82979.0) {
        return (829.79 * x + 54.51) / 255.0;
    } else if (x < 20049.0 / 82979.0) {
        return 127.0 / 255.0;
    } else if (x < 327013.0 / 810990.0) {
        return (792.02249341361393720147485376583 * x - 64.364790735602331034989206222672) / 255.0;
    } else {
        return 1.0;
    }
}

vec4 colormap(float x) {
    return vec4(colormapRed(x), colormapGreen(x), colormapBlue(x), 1.0);
}

float noise(in vec2 x) {
    vec2 p = floor(x);
    vec2 f = fract(x);
    f = f*f*(3.0-2.0*f);

    float a = texture(sampler2D(warpNoiseSmp, warpSmp), (p + vec2(0.5, 0.5)) / 64.0).x;
    float b = texture(sampler2D(warpNoiseSmp, warpSmp), (p + vec2(1.5, 0.5)) / 64.0).x;
    float c = texture(sampler2D(warpNoiseSmp, warpSmp), (p + vec2(0.5, 1.5)) / 64.0).x;
    float d = texture(sampler2D(warpNoiseSmp, warpSmp), (p + vec2(1.5, 1.5)) / 64.0).x;

    return mix(mix( a, b,f.x), mix( c, d,f.x),f.y);
}

const mat2 mtx = mat2(0.80, 0.60, -0.60, 0.80);

float fbm(vec2 p) {
    float f = 0.0;

    f += 0.500000*noise( p + time  ); p = mtx*p*2.02;
    f += 0.031250*noise( p ); p = mtx*p*2.01;
    f += 0.250000*noise( p ); p = mtx*p*2.03;
    f += 0.125000*noise( p ); p = mtx*p*2.01;
    f += 0.062500*noise( p ); p = mtx*p*2.04;
    f += 0.015625*noise( p + sin(time) );

    return f/0.96875;
}

float pattern( in vec2 p ) {
	return fbm( p + fbm( p + fbm( p ) ) );
}

void main() {
    //vec2 _uv = uv / width;
	float shade = pattern(uv);
    fragColor = vec4(colormap(shade).rgb, opacity);
}

@end
@program warp warp_vs warp_fs
