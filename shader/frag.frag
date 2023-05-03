#version 460 core

out vec4 fragColor;
in vec4 pixColor;
in vec2 pixTextCoord;
uniform sampler2D texture0;
void main()
{
    fragColor = texture(texture0,pixTextCoord);
}