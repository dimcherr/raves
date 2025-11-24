@ctype mat4 glm::mat4
@ctype mat3 glm::mat3
@ctype vec2 glm::vec2
@ctype vec3 glm::vec3
@ctype vec4 glm::vec4



@vs turbulence_vs

layout (binding = 0) uniform turbulence_vs_params {
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



@fs turbulence_fs

const float PI = 3.14159265358979323846;

layout (binding = 1) uniform turbulence_fs_params {
    float time;
    vec4 tint;
};

in vec2 uv;

out vec4 fragColor;

/*
    "Turbulent Dark" by @XorDev

    For my tutorial on Turbulence:
    https://mini.gmshaders.com/p/turbulence
    
    
    Simulating proper fluid dynamics can be complicated, limited, and requires a multi-pass setup.

    Sometimes you just want some smoke, fire, or fluid, and you don't want to go through all that trouble.

    This method is very simple! Start with pixel coordinates and scale them down as desired,
    then loop through adding waves, rotating the wave direction and increasing the frequency.
    To animate it, you can add a time offset to the sine wave.
    It also helps to shift each iteration with the iterator "i" to break up visible patterns.

    The resulting coordinates will appear turbulent, and you can use these coordinates in a coloring function.
    
    Smooth, continious equations look best!
*/

//Number of turbulence waves
#define TURB_NUM 10.0
//Turbulence wave amplitude
#define TURB_AMP 0.7
//Turbulence wave speed
#define TURB_SPEED 0.3
//Turbulence frequency
#define TURB_FREQ 2.0
//Turbulence frequency multiplier
#define TURB_EXP 1.4

vec2 turbulence(vec2 p) {
    //Turbulence starting scale
    float freq = TURB_FREQ;
    
    //Turbulence rotation matrix
    mat2 rot = mat2(0.6, -0.8, 0.8, 0.6);
    
    //Loop through turbulence octaves
    for(float i=0.0; i<TURB_NUM; i++)
    {
        //Scroll along the rotated y coordinate
        float phase = freq * (p * rot).y + TURB_SPEED*time + i;
        //Add a perpendicular sine wave offset
        p += TURB_AMP * rot[0] * sin(phase) / freq;
        
        //Rotate for the next octave
        rot *= mat2(0.6, -0.8, 0.8, 0.6);
        //Scale down for the next octave
        freq *= TURB_EXP;
    }
    
    return p;
}

void main() {
    //Screen coordinates, centered and aspect corrected
    //vec2 p = 2.0*(fragCoord.xy*2.0-iResolution.xy)/iResolution.y;
    
    //Apply Turbulence
    vec2 p = turbulence(uv);
    
    //Subtle blue and yellow gradient
    vec3 col = 0.5*exp(0.1*p.x*vec3(-1,0,2));
    //Vary brightness
    col /= dot(cos(p*3.),sin(-p.yx*3.*.618))+2.0;
    //Exponential tonemap
    col = 1.0 - exp(-col);
    fragColor = vec4(col,1) * tint;
}

@end



@program turbulence turbulence_vs turbulence_fs
