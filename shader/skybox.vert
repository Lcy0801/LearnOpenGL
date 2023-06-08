#version 460 core

layout(location = 0) in vec3 aPos;
out vec3 fragTextcoord;

uniform mat4 view;
uniform mat4 project;

void main()
{
    fragTextcoord = aPos;
    vec4 pos = project*view*vec4(aPos.x, aPos.y, aPos.z, 1.0);
    gl_Position = pos.xyww;
}
