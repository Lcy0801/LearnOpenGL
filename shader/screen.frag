#version 460 core

in vec2 fragTextcoord;
uniform sampler2D  textureUnit;

out vec4 fragColor;

const float offset = 0.01;

void main()
{
    vec2 offsets[9]={
        vec2(-offset,offset),
        vec2(0.0f,offset),
        vec2(offset,offset),
        vec2(-offset,0),
        vec2(0,0),
        vec2(offset,0),
        vec2(-offset,-offset),
        vec2(0,-offset),
        vec2(offset,-offset)
    };
    float kernel[9]={
        -1.0f,-1.0f,-1.0f,
        -1.0f,9.0f,-1.0f,
        -1.0f,-1.0f,-1.0f
    };
    vec3 sampleColor;
    for(int i =0;i<9;i++)
    {
        sampleColor += vec3(texture(textureUnit,(fragTextcoord + offsets[i]))*kernel[i]);
    }
    fragColor = vec4(sampleColor,1);
}