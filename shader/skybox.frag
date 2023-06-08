#version 460 core

// in vec3 fragTextcoord;
// uniform samplerCube textureUnit;

out vec4 fragColor;

void main()
{
    //  fragColor = texture(textureUnit,fragTextcoord);
    fragColor = vec4(1,0,0,1);
}