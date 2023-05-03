#version 460 core

layout (location = 0) in vec3 aPos;
layout(location = 1) in vec2 aTextcoord;
out vec4 pixColor;
out vec2 pixTextCoord;
uniform mat4 model;
uniform mat4 view;
uniform mat4 project;
void main()
{
   gl_Position = project*view*model*vec4(aPos.x, aPos.y, aPos.z, 1.0);
   pixTextCoord = aTextcoord;
}