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
    // 点光源
    vec3 lightPos;
    // 平行光
    vec3 lightDir;
    // 引入点光源的衰减系数
    float constant;
    float linear;
    float quadratic;
    // 引入光切角 实现聚光效果
    float cutOff;
    // 引入外光切角 平滑边缘
    float outerCutOff;
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
    float theta;
    theta = dot(normalize(fragPos-light.lightPos),normalize(light.lightDir));
    float intensity = (theta-light.outerCutOff)/(light.cutOff-light.outerCutOff);
    intensity = clamp(intensity,0,1);
    // 环境光
    vec3 ambientRes = light.ambient*light.lightColor*material.ambient;
    // 漫反射光照
    float r = distance(light.lightPos,fragPos);
    // 计算衰减后的光强
    float attenuation = 1/(1+light.constant+light.linear*r+light.quadratic*pow(r,2));
    vec3 lightDir = normalize(light.lightPos-fragPos);
    vec3 fragNormalW = normalize(normalMatrix * fragNormal);
    vec3 diffuseRes = light.lightColor*light.diffuse*attenuation*max(dot(lightDir,fragNormalW),0)*vec3(texture(material.diffuseMap,fragTextcoord));
    // 镜面反射分量
    vec3 viewDir = normalize(cameraPos-fragPos);
    // 计算半程向量
    vec3 h = normalize(viewDir+ lightDir);
    vec3 specularRes = light.lightColor*light.specular*attenuation*vec3(texture(material.specularMap,fragTextcoord))*pow(max(dot(h,fragNormalW),0),material.shininess);
    // 分量叠加
    fragColor =vec4(ambientRes+intensity*diffuseRes+intensity*specularRes,1); 
}