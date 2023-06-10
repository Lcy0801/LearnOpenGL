#version 460 core

in vec3 fragPos;
in vec3 fragNormal;
in vec2 fragTextcoord;

uniform mat3 normalMatrix;
uniform samplerCube skybox;
uniform vec3 cameraPos;

out vec4 fragColor;

void main()
{
    vec3 fragNormalW = normalize(normalMatrix*fragNormal);
    vec3 viewDir = normalize(fragPos - cameraPos);
    vec3 lightDir = reflect(viewDir,fragNormalW);
    fragColor = texture(skybox,lightDir);
}