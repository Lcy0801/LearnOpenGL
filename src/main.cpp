#include <iostream>
#include <fstream>
#include <sstream>
#include <string.h>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <math.h>
#include <ctime>
#include <Shader.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

// 引入标准图像库 加载纹理贴图
#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>
#define screenWidth 800
#define screenHeight 800

using namespace std;
using namespace glm;

// 定义相机参数相关的全局变量
vec3 cameraPos = vec3(0, 0, 3);
vec3 cameraFront = vec3(0, 0, -1);
vec3 cameraUp = vec3(0, 1, 0);
float cameraSpeed = 10;
// 相邻两帧的时间间隔 用于平衡不同性能机器渲染时相机的移动速度
float deltaTime = 0, lastTime = 0;

void framebuffer_size_callback(GLFWwindow *window, int width, int height)
{
    if (window)
    {
        glViewport(0, 0, width, height);
    }
}

void processInput(GLFWwindow *window)
{
    // 关闭窗口
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
    {
        glfwSetWindowShouldClose(window, true);
    }
    // 前进
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
    {
        cameraPos = cameraPos + cameraFront * cameraSpeed * deltaTime;
    }
    // 后退
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
    {
        cameraPos = cameraPos - cameraFront * cameraSpeed * deltaTime;
    }
    // 左移
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
    {
        cameraPos = cameraPos + normalize(cross(cameraUp, cameraFront)) * cameraSpeed * deltaTime;
    }
    // 右移
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
    {
        cameraPos = cameraPos + normalize(cross(cameraFront, cameraUp)) * cameraSpeed * deltaTime;
    }
}

int main()
{

    // 创建窗口
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    GLFWwindow *window = glfwCreateWindow(screenWidth, screenHeight, "LearnOpenGL", NULL, NULL);
    if (window == NULL)
    {
        cout << "初始化窗口失败!" << endl;
        glfwTerminate();
        exit(-1);
    }
    glfwMakeContextCurrent(window);
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        cout << "GLAD初始化失败!" << endl;
        glfwTerminate(); // 释放资源
        exit(-1);
    }
    glViewport(0, 0, screenWidth, screenHeight);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    Shader shader("../shader/vertex.vert", "../shader/frag.frag");
    float vertices[] = {
        -0.5f, -0.5f, -0.5f, 0.0f, 0.0f,
        0.5f, -0.5f, -0.5f, 1.0f, 0.0f,
        0.5f, 0.5f, -0.5f, 1.0f, 1.0f,
        0.5f, 0.5f, -0.5f, 1.0f, 1.0f,
        -0.5f, 0.5f, -0.5f, 0.0f, 1.0f,
        -0.5f, -0.5f, -0.5f, 0.0f, 0.0f,

        -0.5f, -0.5f, 0.5f, 0.0f, 0.0f,
        0.5f, -0.5f, 0.5f, 1.0f, 0.0f,
        0.5f, 0.5f, 0.5f, 1.0f, 1.0f,
        0.5f, 0.5f, 0.5f, 1.0f, 1.0f,
        -0.5f, 0.5f, 0.5f, 0.0f, 1.0f,
        -0.5f, -0.5f, 0.5f, 0.0f, 0.0f,

        -0.5f, 0.5f, 0.5f, 1.0f, 0.0f,
        -0.5f, 0.5f, -0.5f, 1.0f, 1.0f,
        -0.5f, -0.5f, -0.5f, 0.0f, 1.0f,
        -0.5f, -0.5f, -0.5f, 0.0f, 1.0f,
        -0.5f, -0.5f, 0.5f, 0.0f, 0.0f,
        -0.5f, 0.5f, 0.5f, 1.0f, 0.0f,

        0.5f, 0.5f, 0.5f, 1.0f, 0.0f,
        0.5f, 0.5f, -0.5f, 1.0f, 1.0f,
        0.5f, -0.5f, -0.5f, 0.0f, 1.0f,
        0.5f, -0.5f, -0.5f, 0.0f, 1.0f,
        0.5f, -0.5f, 0.5f, 0.0f, 0.0f,
        0.5f, 0.5f, 0.5f, 1.0f, 0.0f,

        -0.5f, -0.5f, -0.5f, 0.0f, 1.0f,
        0.5f, -0.5f, -0.5f, 1.0f, 1.0f,
        0.5f, -0.5f, 0.5f, 1.0f, 0.0f,
        0.5f, -0.5f, 0.5f, 1.0f, 0.0f,
        -0.5f, -0.5f, 0.5f, 0.0f, 0.0f,
        -0.5f, -0.5f, -0.5f, 0.0f, 1.0f,

        -0.5f, 0.5f, -0.5f, 0.0f, 1.0f,
        0.5f, 0.5f, -0.5f, 1.0f, 1.0f,
        0.5f, 0.5f, 0.5f, 1.0f, 0.0f,
        0.5f, 0.5f, 0.5f, 1.0f, 0.0f,
        -0.5f, 0.5f, 0.5f, 0.0f, 0.0f,
        -0.5f, 0.5f, -0.5f, 0.0f, 1.0f};
    // 绘制多个矩形
    vec3 cubePositions[] = {
        vec3(0.0f, 0.0f, 0.0f),
        vec3(2.0f, 5.0f, -15.0f),
        vec3(-1.5f, -2.2f, -2.5f),
        vec3(-3.8f, -2.0f, -12.3f),
        vec3(2.4f, -0.4f, -3.5f),
        vec3(-1.7f, 3.0f, -7.5f),
        vec3(1.3f, -2.0f, -2.5f),
        vec3(1.5f, 2.0f, -2.5f),
        vec3(1.5f, 0.2f, -1.5f),
        vec3(-1.3f, 1.0f, -1.5f)};
    // 顶点数组对象
    unsigned int vao;
    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);
    unsigned int vbo;
    glGenBuffers(1, &vbo);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    // 链接着色器顶点属性与顶点数据的对应关系
    glVertexAttribPointer(0, 3, GL_FLOAT, false, 5 * sizeof(GL_FLOAT), 0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 2, GL_FLOAT, false, 5 * sizeof(GL_FLOAT), (void *)(3 * sizeof(GL_FLOAT)));
    glEnableVertexAttribArray(1);
    glBindVertexArray(0);
    // 背景
    unsigned int vaobg;
    glGenVertexArrays(1, &vaobg);
    glBindVertexArray(vaobg);
    unsigned int vbobg;
    glGenBuffers(1, &vbobg);
    glBindBuffer(GL_ARRAY_BUFFER, vbobg);
    float bgverices[] = {
        -1, -1, 0.999999, 0, 1,
        1, -1, 0.999999, 1, 1,
        1, 1, 0.999999, 1, 0,
        -1, 1, 0.999999, 0, 0};
    glBufferData(GL_ARRAY_BUFFER, sizeof(bgverices), bgverices, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, false, 5 * sizeof(GL_FLOAT), 0);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, false, 5 * sizeof(GL_FLOAT), (void *)(3 * sizeof(GL_FLOAT)));
    unsigned int ibobg;
    glGenBuffers(1, &ibobg);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibobg);
    unsigned int bgindices[] = {0, 1, 2, 0, 2, 3};
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(bgindices), bgindices, GL_STATIC_DRAW);

    // 绑定默认vao
    glBindVertexArray(0);
    auto loadTexture = [](unsigned int &texture, const char *imageFile, int TextureUnit)
    {
        // 纹理
        glGenTextures(1, &texture);
        glActiveTexture(TextureUnit);
        glBindTexture(GL_TEXTURE_2D, texture);
        // 设置纹理环绕方式
        glTextureParameteri(texture, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTextureParameteri(texture, GL_TEXTURE_WRAP_T, GL_REPEAT);
        // 设置纹理过滤方式
        glTextureParameteri(texture, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTextureParameteri(texture, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        // 纹理贴图
        int width, height, nChannels;
        unsigned char *textureData = stbi_load(imageFile, &width, &height, &nChannels, 0);
        if (textureData)
        {
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, textureData);
            glGenerateMipmap(GL_TEXTURE_2D);
            // 释放贴图内存
            stbi_image_free(textureData);
        }
        else
        {
            cout << imageFile << endl;
            cout << "该纹理贴图载入失败!" << endl;
        }
    };
    // 纹理
    unsigned int texture0;
    loadTexture(texture0, "D:/LearnOpenGL/textures/0.png", GL_TEXTURE0);
    unsigned int texture1;
    loadTexture(texture1, "D:/LearnOpenGL/textures/1.png", GL_TEXTURE1);
    unsigned int texture2;
    loadTexture(texture2, "D:/LearnOpenGL/textures/2.png", GL_TEXTURE2);
    unsigned int texture3;
    loadTexture(texture3, "D:/LearnOpenGL/textures/3.png", GL_TEXTURE3);
    unsigned int texture4;
    loadTexture(texture4, "D:/LearnOpenGL/textures/4.png", GL_TEXTURE4);
    unsigned int texture5;
    loadTexture(texture5, "D:/LearnOpenGL/textures/5.png", GL_TEXTURE5);
    unsigned int texture6;
    loadTexture(texture6, "D:/LearnOpenGL/textures/6.png", GL_TEXTURE6);
    unsigned int texture7;
    loadTexture(texture7, "D:/LearnOpenGL/textures/7.png", GL_TEXTURE7);
    unsigned int texture8;
    loadTexture(texture8, "D:/LearnOpenGL/textures/8.png", GL_TEXTURE8);
    unsigned int texture9;
    loadTexture(texture9, "D:/LearnOpenGL/textures/9.png", GL_TEXTURE9);
    unsigned int texture10;
    loadTexture(texture10, "D:/LearnOpenGL/textures/10.png", GL_TEXTURE10);
    unsigned int texture11;
    loadTexture(texture11, "D:/LearnOpenGL/textures/11.png", GL_TEXTURE11);
    unsigned int texturebg;
    loadTexture(texturebg, "D:/LearnOpenGL/textures/bg.png", GL_TEXTURE12);

    // 开启深度测试
    glEnable(GL_DEPTH_TEST);
    while (!glfwWindowShouldClose(window))
    {
        processInput(window);
        glClearColor(1.0, 1.0, 1.0, 1.0);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        shader.use();
        glBindVertexArray(vaobg);
        mat4 model, view, project;
        glUniformMatrix4fv(glGetUniformLocation(shader.ID, "model"), 1, false, value_ptr(model));
        glUniformMatrix4fv(glGetUniformLocation(shader.ID, "view"), 1, false, value_ptr(view));
        glUniformMatrix4fv(glGetUniformLocation(shader.ID, "project"), 1, false, value_ptr(project));
        shader.setUniformInt("texture0", 12);
        shader.setUniformInt("texture1", 12);
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
        glBindVertexArray(vao);
        view = lookAt(cameraPos, cameraPos + cameraFront, cameraUp);
        project = perspective(45.0f, (float)screenWidth / screenHeight, 0.1f, 100.0f);
        glUniformMatrix4fv(glGetUniformLocation(shader.ID, "view"), 1, false, value_ptr(view));
        glUniformMatrix4fv(glGetUniformLocation(shader.ID, "project"), 1, false, value_ptr(project));
        for (int i = 0; i < 10; i++)
        {
            // 空间变换
            mat4 model;
            float timeValue = glfwGetTime();
            model = translate(model, cubePositions[i]);
            model = rotate(model, timeValue + radians(i * 30.0f), vec3(1, 1, 1));
            glUniformMatrix4fv(glGetUniformLocation(shader.ID, "model"), 1, false, value_ptr(model));
            float mixPercent = sin(timeValue) * 0.5 + 0.5;
            shader.setUniformFloat("mixPercent", mixPercent);
            for (int j = 0; j < 6; j++)
            {
                // 设置着色器sampler 一定要先激活着色器程序
                shader.use();
                shader.setUniformInt("texture0", 2 * j);
                shader.setUniformInt("texture1", 2 * j + 1);
                glDrawArrays(GL_TRIANGLES, j * 6, 6);
            }
        }
        glfwSwapBuffers(window);
        float currentTime = glfwGetTime();
        deltaTime = currentTime - lastTime;
        lastTime = currentTime;
        glfwPollEvents();
    }
    glDeleteVertexArrays(1, &vao);
    glDeleteBuffers(1, &vbo);
    shader.destroy();
    glDeleteTextures(1, &texture0);
    glfwTerminate();
    return 0;
}