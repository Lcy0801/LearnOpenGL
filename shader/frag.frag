#version 460 core

out vec4 fragColor;
uniform float ambientStrength;
uniform float specularStrength;
uniform vec3 lightColor;
uniform vec3 lightPos;
uniform mat3 normalMatrix;
uniform vec3 cameraPos;
uniform sampler2D texture0;

in vec3 fragPos;
in vec3 fragNormal;
in vec2 fragTextcoord;
void main()
{
    vec3 objColor = vec3(texture(texture0,fragTextcoord));
    // 环境光
    vec3 ambient = ambientStrength*lightColor;
    vec3 ambientRes = ambient * objColor;
    // 漫反射光照
    float r = distance(lightPos,fragPos);
    vec3 lightDir = normalize(lightPos-fragPos);
    vec3 fragNormalW = normalize(normalMatrix * fragNormal);
    vec3 diffuseRes = lightColor/pow(r,2)*max(dot(lightDir,fragNormalW),0)*objColor;
    // 镜面反射分量
    vec3 viewDir = normalize(cameraPos-fragPos);
    // 计算半程向量
    vec3 h = normalize(viewDir+ lightDir);
    vec3 specularRes = lightColor/pow(r,2)*specularStrength*pow(max(dot(h,fragNormalW),0),9);
    // 分量叠加
    fragColor =vec4(ambientRes,1)+vec4(diffuseRes,1)+vec4(specularRes,1); 
}