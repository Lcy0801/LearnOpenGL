#version 460 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec2 aTextcoord;
out vec2 fragTextcoord;
void main()
{
   gl_Position = vec4(aPos,1);
   fragTextcoord = aTextcoord;
}