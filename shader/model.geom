#version 460 core

layout(triangles) in;
layout(triangle_strip,max_vertices = 3) out;

in VS_Out
{
    vec2 texCoords;
} gs_in[];

out vec2 texCoords;
uniform float time;

vec3 getNormal()
{
    
}


