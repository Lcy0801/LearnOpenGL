#version 460 core

in vec2 fragTextcoord;
uniform sampler2D  textureUnit;

out vec4 fragColor;
void main()
{
    fragColor = texture(textureUnit,fragTextcoord);
}