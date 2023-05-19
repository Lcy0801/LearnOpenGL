#version 460 core

layout (location = 0) in vec3 aPos;
layout (location = 2) in vec2 aTextcoord;
uniform mat4 model;
uniform mat4 view;
uniform mat4 project;
out vec2 fragTextcoord;
void main()
{
   gl_Position = project*view*model*vec4(aPos.x, aPos.y, aPos.z, 1.0);
   fragTextcoord = aTextcoord;
}