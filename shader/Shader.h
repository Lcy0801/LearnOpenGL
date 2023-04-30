#ifndef SHADER_H
#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <glad/glad.h>
#include <GLFW/glfw3.h>

class Shader
{
public:
    unsigned int ID;
    Shader(const char *vertexFile, const char *fragFile)
    {
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
};

#endif