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
    // ���Դ
    vec3 position;
    // ƽ�й�
    vec3 direction;
    // ������Դ��˥��ϵ��
    float constant;
    float linear;
    float quadratic;
    // ������н� ʵ�־۹�Ч��
    float cutOff;
    // ��������н� ƽ����Ե
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
    theta = dot(normalize(fragPos-light.position),normalize(light.direction));
    float intensity = (theta-light.outerCutOff)/(light.cutOff-light.outerCutOff);
    intensity = clamp(intensity,0,1);
    // ������
    vec3 ambientRes = light.ambient*material.ambient;
    // ���������
    float r = distance(light.position,fragPos);
    // ����˥����Ĺ�ǿ
    float attenuation = 1/(1+light.constant+light.linear*r+light.quadratic*pow(r,2));
    vec3 lightDir = normalize(light.position-fragPos);
    vec3 fragNormalW = normalize(normalMatrix * fragNormal);
    vec3 diffuseRes = light.diffuse*attenuation*max(dot(lightDir,fragNormalW),0)*vec3(texture(material.diffuseMap,fragTextcoord));
    // ���淴�����
    vec3 viewDir = normalize(cameraPos-fragPos);
    // ����������
    vec3 h = normalize(viewDir+ lightDir);
    vec3 specularRes = light.specular*attenuation*vec3(texture(material.specularMap,fragTextcoord))*pow(max(dot(h,fragNormalW),0),material.shininess);
    // ��������
    fragColor =vec4(ambientRes+intensity*diffuseRes+intensity*specularRes,1); 
}