@ctype mat4 glm::mat4
@ctype mat3 glm::mat3
@ctype vec2 glm::vec2
@ctype vec3 glm::vec3
@ctype vec4 glm::vec4



@vs organic_vs

layout (binding = 0) uniform organic_vs_params {
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



@fs organic_fs

const float PI = 3.14159265358979323846;

layout (binding = 1) uniform organic_fs_params {
    float time;
    float opacity;
};

in vec2 uv;

out vec4 fragColor;

vec3 palette (float t) {
    vec3 a = vec3(.5, 0.1, 1.);
    vec3 b = vec3(0.1, 0.5, 0.5);
    vec3 c = vec3(2.0, 0.0, 1.0);
    vec3 d = vec3(6.263,3.416,0.5);

    return a + b*cos( 1.5*(c*t+d) );
}

mat2 rotate2D(float r) {
    return mat2(cos(r), sin(r), -sin(r), cos(r));
}

void main() {
    vec3 col = palette(time * 2.0);
    float t = time;
    
    vec2 n = vec2(0);
    vec2 q = vec2(0);
    vec2 k = fract(uv*1.50)-0.5;
    vec2 p = uv; 
    float d = dot(p,p);
    float S = 5.; //size of blob
    float a = -cos(time)*0.001; //brightness 
    mat2 m = rotate2D(1.0);

    for (float j = 0.; j < 8; j++) {
        p *= m;
        n *= m;
        q = p*S+j*2.+n;//image
        q += cos(t * 0.5) * sin(t * 0.5); // movement
        a += dot(cos(q)/S, vec2(0.25));
        n -= sin(q);
        S *= 1.2;
    }
    
    col *= (a +.8)+ 2.*a-d;

    fragColor = vec4(col, opacity);
}

@end
@program organic organic_vs organic_fs
