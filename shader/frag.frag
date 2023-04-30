#version 460 core

out vec4 fragColor;
in vec4 pixColor;
in vec2 pixTextCoord;
uniform sampler2D texture0;
uniform sampler2D texture1;
uniform float mixPercent;

void main()
{
    fragColor = mix(texture(texture0,pixTextCoord),texture(texture1,pixTextCoord),mixPercent);
}