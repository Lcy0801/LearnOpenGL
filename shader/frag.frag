#version 460 core

out vec4 fragColor;
uniform vec3 objColor;
uniform vec3 lightColor;
void main()
{
    fragColor = vec4(objColor,1)*vec4(lightColor,1);
}