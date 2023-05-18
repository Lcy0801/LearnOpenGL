#version 460 core

uniform vec3 borderColor;
out vec4 fragColor;
void main()
{
    fragColor = vec4(borderColor,1);
}