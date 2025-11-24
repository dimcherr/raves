@ctype mat4 glm::mat4
@ctype mat3 glm::mat3
@ctype vec2 glm::vec2
@ctype vec3 glm::vec3
@ctype vec4 glm::vec4



@vs snowy_vs

layout (binding = 0) uniform snowy_vs_params {
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



@fs snowy_fs

const float PI = 3.14159265358979323846;

layout (binding = 1) uniform snowy_fs_params {
    float time;
    float opacity;
};

in vec2 uv;

out vec4 fragColor;

void main() {
    float snow = 0.0;
    float gradient = (1.0-uv.y)*0.4;
    float random = fract(sin(dot(uv,vec2(12.9898,78.233)))* 43758.5453);
    for(int k=0;k<6;k++){
        for(int i=0;i<12;i++){
            float cellSize = 2.0 + (float(i)*3.0);
			float downSpeed = 0.3+(sin(time*0.4+float(k+i*20))+1.0)*0.00008;
            vec2 _uv = uv +vec2(0.01*sin((time+float(k*6185))*0.6+float(i))*(5.0/float(i)),downSpeed*(time+float(k*1352))*(1.0/float(i)));
            vec2 uvStep = (ceil((_uv)*cellSize-vec2(0.5,0.5))/cellSize);
            float x = fract(sin(dot(uvStep.xy,vec2(12.9898+float(k)*12.0,78.233+float(k)*315.156)))* 43758.5453+float(k)*12.0)-0.5;
            float y = fract(sin(dot(uvStep.xy,vec2(62.2364+float(k)*23.0,94.674+float(k)*95.0)))* 62159.8432+float(k)*12.0)-0.5;

            float randomMagnitude1 = sin(time*2.5)*0.7/cellSize;
            float randomMagnitude2 = cos(time*2.5)*0.7/cellSize;

            float d = 5.0*distance((uvStep.xy + vec2(x*sin(y),y)*randomMagnitude1 + vec2(y,x)*randomMagnitude2),_uv.xy);

            float omiVal = fract(sin(dot(uvStep.xy,vec2(32.4691,94.615)))* 31572.1684);
            if(omiVal<0.08?true:false){
                float newd = (x+1.0)*0.4*clamp(1.9-d*(15.0+(x*6.3))*(cellSize/1.4),0.0,1.0);
                snow += newd;
            }
        }
    }
    
    vec4 col = vec4(snow) + gradient*vec4(0.4,0.6,0.8,0.0) + random*0.01;
    fragColor = vec4(col.rgb, opacity);
}

@end
@program snowy snowy_vs snowy_fs
