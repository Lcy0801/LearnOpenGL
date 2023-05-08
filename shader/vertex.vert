#version 460 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTextcoord;
uniform mat4 model;
uniform mat4 view;
uniform mat4 project;
out vec3 fragPos;
out vec3 fragNormal;
void main()
{
   gl_Position = project*view*model*vec4(aPos.x, aPos.y, aPos.z, 1.0);
   fragPos = vec3(model*vec4(aPos,1));
   fragNormal = aNormal;
}