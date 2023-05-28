#version 460 core

in vec2 fragTextcoord;
uniform sampler2D  textureUnit;

out vec4 fragColor;
void main()
{
    fragColor = vec4(vec3(1.0-texture(textureUnit,fragTextcoord)),1);
    // fragColor = texture(textureUnit,fragTextcoord);
}