#ifndef SHADER_H
#define SHADER_H
#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

struct DirLight
{
    glm::vec3 direction;
    glm::vec3 ambient;
    glm::vec3 diffuse;
    glm::vec3 specular;
};

struct PoiLight
{
    glm::vec3 position;
    glm::vec3 ambient;
    glm::vec3 diffuse;
    glm::vec3 specular;
    float constant;
    float linear;
    float quadratic;
    float cutOff = 0;
    float outerCutOff;
    glm::vec3 direction;
};

struct Material
{
    glm::vec3 ambient;
    glm::vec3 diffuse;
    int diffuseMap = -1;
    glm::vec3 specular;
    int specularMap = -1;
    int shininess;
};

class Shader
{
public:
    unsigned int ID;
    Shader(const char *vertexFile, const char *fragFile)
    {
        std::cout << vertexFile << std::endl;
        std::cout << fragFile << std::endl;
        std::ifstream vertexIFS, fragIFS;
        vertexIFS.exceptions(std::ifstream::failbit | std::ifstream::badbit);
        fragIFS.exceptions(std::ifstream::failbit | std::ifstream::badbit);
        std::stringstream vertexSS, fragSS;
        try
        {
            vertexIFS.open(vertexFile);
            fragIFS.open(fragFile);
            vertexSS << vertexIFS.rdbuf();
            fragSS << fragIFS.rdbuf();
            vertexIFS.close();
            fragIFS.close();
        }
        catch (std::ifstream::failure e)
        {
            std::cout << "着色器初始化失败!" << std::endl;
            return;
        }
        std::string vertexCode;
        std::string fragCode;
        vertexCode = vertexSS.str();
        fragCode = fragSS.str();
        const char *vertexCode_ = vertexCode.c_str();
        const char *fragCode_ = fragCode.c_str();
        // 编译链接着色器
        int flag;
        char infoLog[1024];
        // 顶点着色器
        unsigned int vertexShader = glCreateShader(GL_VERTEX_SHADER);
        glShaderSource(vertexShader, 1, &vertexCode_, nullptr);
        glCompileShader(vertexShader);
        glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &flag);
        if (!flag)
        {
            glGetShaderInfoLog(vertexShader, 1024, nullptr, infoLog);
            std::cout << "顶点着色器编译失败!" << std::endl;
            std::cout << infoLog << std::endl;
            exit(-1);
        }
        // 片段着色器
        unsigned int fragShader;
        fragShader = glCreateShader(GL_FRAGMENT_SHADER);
        glShaderSource(fragShader, 1, &fragCode_, nullptr);
        glCompileShader(fragShader);
        glGetShaderiv(fragShader, GL_COMPILE_STATUS, &flag);
        if (!flag)
        {
            glGetShaderInfoLog(fragShader, 1024, nullptr, infoLog);
            std::cout << "片段着色器编译失败!" << std::endl;
            std::cout << infoLog << std::endl;
            exit(-1);
        }
        // 着色器程序
        ID = glCreateProgram();
        glAttachShader(ID, vertexShader);
        glAttachShader(ID, fragShader);
        glLinkProgram(ID);
        glGetProgramiv(ID, GL_LINK_STATUS, &flag);
        if (!flag)
        {
            glGetProgramInfoLog(ID, 1024, nullptr, infoLog);
            std::cout << "着色器链接失败!" << std::endl;
            std::cout << infoLog << std::endl;
            exit(-1);
        }
        glDeleteShader(vertexShader);
        glDeleteShader(fragShader);
    }
    void use()
    {
        glUseProgram(ID);
    }
    void setUniformBool(const std::string &uniformName, bool uniformValue)
    {
        int uniformLocation = glGetUniformLocation(ID, uniformName.c_str());
        if (uniformLocation != -1)
        {
            glUniform1i(uniformLocation, (int)uniformValue);
        }
        else
        {
            std::cout << uniformName << "不存在!" << std::endl;
        }
    }
    void setUniformInt(const std::string &uniformName, int uniformValue)
    {
        int uniformLocation = glGetUniformLocation(ID, uniformName.c_str());
        if (uniformLocation != -1)
        {
            glUniform1i(uniformLocation, uniformValue);
        }
        else
        {
            std::cout << uniformName << "不存在!" << std::endl;
        }
    }
    void setUniformFloat(const std::string &uniformName, float uniformValue)
    {
        int uniformLocation = glGetUniformLocation(ID, uniformName.c_str());
        if (uniformLocation != -1)
        {
            glUniform1f(uniformLocation, uniformValue);
        }
        else
        {
            std::cout << uniformName << "不存在!" << std::endl;
        }
    }
    void setUniformVec3(const std::string &uniformName, const glm::vec3 &uniformValue)
    {
        int uniformLocation = glGetUniformLocation(ID, uniformName.c_str());
        if (uniformLocation != -1)
        {
            glUniform3f(uniformLocation, uniformValue[0], uniformValue[1], uniformValue[2]);
        }
        else
        {
            std::cout << uniformName << "不存在!" << std::endl;
        }
    }
    void setUniformMatrix4(const std::string &uniformName, const glm::mat4 &uniformValue)
    {
        int uniformLocation = glGetUniformLocation(ID, uniformName.c_str());
        if (uniformLocation != -1)
        {
            glUniformMatrix4fv(uniformLocation, 1, false, glm::value_ptr(uniformValue));
        }
        else
        {
            std::cout << uniformName << "不存在!" << std::endl;
        }
    }
    void setUniformMatrix3(const std::string &uniformName, const glm::mat3 &uniformValue)
    {
        int uniformLocation = glGetUniformLocation(ID, uniformName.c_str());
        if (uniformLocation != -1)
        {
            glUniformMatrix3fv(uniformLocation, 1, false, glm::value_ptr(uniformValue));
        }
        else
        {
            std::cout << uniformName << "不存在!" << std::endl;
        }
    }

    void setUniformDirLight(const std::string &uniformName, const DirLight &dirLight)
    {
        std::string direction;
        direction = uniformName + ".direction";
        setUniformVec3(direction, dirLight.direction);
        std::string ambient;
        ambient = uniformName + ".ambient";
        setUniformVec3(ambient, dirLight.ambient);
        std::string diffuse;
        diffuse = uniformName + ".diffuse";
        setUniformVec3(diffuse, dirLight.diffuse);
        std::string specular;
        specular = uniformName + ".specular";
        setUniformVec3(specular, dirLight.specular);
    }

    void setUniformPoiLight(const std::string &uniformName, const PoiLight &poiLight)
    {
        std::string position;
        position = uniformName + ".position";
        setUniformVec3(position, poiLight.position);
        std::string ambient;
        ambient = uniformName + ".ambient";
        setUniformVec3(ambient, poiLight.ambient);
        std::string diffuse;
        diffuse = uniformName + ".diffuse";
        setUniformVec3(diffuse, poiLight.diffuse);
        std::string specular;
        specular = uniformName + ".specular";
        setUniformVec3(specular, poiLight.specular);
        std::string constant;
        constant = uniformName + ".constant";
        setUniformFloat(constant, poiLight.constant);
        std::string linear;
        linear = uniformName + ".linear";
        setUniformFloat(linear, poiLight.linear);
        std::string quadratic;
        quadratic = uniformName + ".quadratic";
        setUniformFloat(quadratic, poiLight.quadratic);
        std::string cutOff;
        cutOff = uniformName + ".cutOff";
        setUniformFloat(cutOff, poiLight.cutOff);
        if (poiLight.cutOff != 0)
        {
            std::string direction;
            direction = uniformName + ".direction";
            setUniformVec3(direction, poiLight.direction);
            std::string outerCutOff;
            outerCutOff = uniformName + ".outerCutOff";
            setUniformFloat(outerCutOff, poiLight.outerCutOff);
        }
    }
    void setUniformMaterail(const std::string &uniformName, const Material &material)
    {
        std::string ambient = uniformName + ".ambient";
        setUniformVec3(ambient, material.ambient);
        if (material.diffuseMap != -1)
        {
            std::string diffuseMap = uniformName + ".diffuseMap";
            setUniformInt(diffuseMap, material.diffuseMap);
        }
        else
        {
            std::string diffuse = uniformName + ".diffuse";
            setUniformVec3(diffuse, material.diffuse);
        }
        if (material.specularMap != -1)
        {
            std::string specularMap = uniformName + ".specularMap";
            setUniformInt(specularMap, material.specularMap);
        }
        else
        {
            std::string specualr = uniformName + ".specular";
            setUniformVec3(specualr, material.specular);
        }
        std::string shininess = uniformName + ".shininess";
        setUniformInt(shininess, material.shininess);
    }
    void destroy()
    {
        glDeleteProgram(ID);
    }
};

#endif