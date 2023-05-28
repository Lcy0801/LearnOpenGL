#version 460 core

in vec2 fragTextcoord;
uniform sampler2D  textureUnit;

out vec4 fragColor;
void main()
{
    fragColor = texture(textureUnit,fragTextcoord);
    float avgGray = 0.2126*fragColor.r+0.7152*fragColor.g+0.0722*fragColor.b;
    fragColor = vec4(avgGray,avgGray,avgGray,1);
}