#version 460 core

struct Material
{
    // �����ⷴ����
    vec3 ambient;
    // ��������
    vec3 diffuse;
    // ��������ͼ
    sampler2D diffuseMap_1;
    // ���淴����
    vec3 specular;
    // ���淴����ͼ
    sampler2D specularMap_1;
    // �����
    int shininess;
};
// �����
struct DirLight 
{
    vec3 direction;
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};
// ���Դ
struct  PointLight
{
    vec3 position;
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
    float constant;
    float linear;
    float quadratic;
    float cutOff;
    float outerCutOff;
    vec3 direction;
};

#define NR_POINT_LIGHTS 4


uniform Material material;
// һ������� �ĸ����Դ һ��������
uniform DirLight dirLight;
uniform PointLight poiLights[NR_POINT_LIGHTS];
uniform PointLight bunchedLight;
// �����λ��
uniform vec3 cameraPos;
// ���߾���
uniform mat3 normalMatrix;
// ������ͼ
uniform float mixPercent;

in vec3 fragPos;
in vec2 fragTextcoord;
in vec3 fragNormal;

out vec4 fragColor;

vec3 calcDirLight(DirLight dirLight,vec3 fragNormalW,vec3 viewDir)
{
    // ������
    vec3 ambientRes = dirLight.ambient*material.ambient;
    // �������
    vec4 diffuse=texture(material.diffuseMap_1,fragTextcoord);
    vec3 diffuseRes = dirLight.diffuse*max(dot(normalize(-dirLight.direction),fragNormalW),0)*vec3(diffuse);
    // ���淴��
    vec3 h = normalize(viewDir-dirLight.direction);
    vec3 specularRes = dirLight.specular*vec3(texture(material.specularMap_1,fragTextcoord))*pow(max(dot(h,fragNormalW),0),material.shininess);
    vec3 dirLightRes = ambientRes+diffuseRes+specularRes;
    return dirLightRes;
}




vec3 calPointLight(PointLight poiLight,vec3 fragNormalW,vec3 viewDir)
{
    // ������
    vec3 ambientRes = poiLight.ambient*material.ambient;
    // ����˥��ϵ��
    float r = distance(poiLight.position,fragPos);
    float attenuation = 1/(1+poiLight.constant+poiLight.linear*r+poiLight.quadratic*pow(r,2));
    // ������
    vec4 diffuse=texture(material.diffuseMap_1,fragTextcoord);
    vec3 lightDir = normalize(poiLight.position - fragPos);
    vec3 diffuseRes = poiLight.diffuse*attenuation*max(dot(lightDir,fragNormalW),0)*vec3(diffuse);
    // ���淴��
    vec3 h = normalize(viewDir+lightDir);
    vec3 specularRes = poiLight.specular*attenuation*vec3(texture(material.specularMap_1,fragTextcoord))*pow(max(dot(h,fragNormalW),0),material.shininess);
    // �ж��Ƿ�Ϊ�۹��
    float intensity=1;
    if(poiLight.cutOff!=0)
    {
        float theta = dot(-lightDir,normalize(poiLight.direction));
        intensity = (theta-poiLight.outerCutOff)/(poiLight.cutOff-poiLight.outerCutOff);
        intensity = clamp(intensity,0,1);
    }
    vec3 poiLightRes = ambientRes+diffuseRes*intensity+specularRes*intensity;
    return poiLightRes;
}


void main()
{
    vec3 fragNormalW = normalize(normalMatrix*fragNormal);
    vec3 viewDir = normalize(cameraPos-fragPos);
    // // �����
    vec3 fragRes;
    fragRes = calcDirLight(dirLight,fragNormalW,viewDir);
    // ���Դ
    for(int i=0;i<NR_POINT_LIGHTS;i++)
    {
        fragRes += calPointLight(poiLights[i],fragNormalW,viewDir);
    }
    // // ������
    fragRes +=calPointLight(bunchedLight,fragNormalW,viewDir);
    fragColor = vec4(fragRes,1);
}