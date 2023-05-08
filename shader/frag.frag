#version 460 core

struct Material
{
    vec3 ambient;
    vec3 diffuse;
    sampler2D diffuseMap;
    vec3 specular;
    sampler2D specularMap;
    int shininess;
};

struct Light
{
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
    vec3 lightColor;
    vec3 lightPos;
};

out vec4 fragColor;
in vec3 fragPos;
in vec3 fragNormal;
in vec2 fragTextcoord;

uniform Light light;
uniform Material material;
uniform mat3 normalMatrix;
uniform vec3 cameraPos;


void main()
{
    // 环境光
    vec3 ambientRes = light.ambient*light.lightColor*material.ambient;
    // 漫反射光照
    float r = distance(light.lightPos,fragPos);
    vec3 lightDir = normalize(light.lightPos-fragPos);
    vec3 fragNormalW = normalize(normalMatrix * fragNormal);
    vec3 diffuseRes = light.lightColor*light.diffuse/pow(r,2)*max(dot(lightDir,fragNormalW),0)*vec3(texture(material.diffuseMap,fragTextcoord));
    // 镜面反射分量
    vec3 viewDir = normalize(cameraPos-fragPos);
    // 计算半程向量
    vec3 h = normalize(viewDir+ lightDir);
    vec3 specularRes = light.lightColor*light.specular/pow(r,2)*vec3(texture(material.specularMap,fragTextcoord))*pow(max(dot(h,fragNormalW),0),material.shininess);
    // 分量叠加
    fragColor =vec4(ambientRes+diffuseRes+specularRes,1); 
}