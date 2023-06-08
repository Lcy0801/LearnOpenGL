#include <iostream>
#include <fstream>
#include <sstream>
#include <string.h>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <math.h>
#include <ctime>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <vector>
#include <Shader.h>
#include <Model.h>
#include <Camera.h>
using namespace std;
using namespace glm;

#define screenWidth 800
#define screenHeight 800

#define X_SEGMENTS 360
#define Y_SEGMENTS 180
// 球形光源的半径大小
#define RAIDUS 1
#define PI 3.1415926
// 点光源的个数
#define NR_POINT_LIGHTS 4

// 定义相机参数相关的全局变量
vec3 cameraPos = vec3(0, 0, 3);
float cameraSpeed = 5;
// 视场大小
// 变小会产生放大的效果 变大会产生缩小的效果
float fov = 45;
// 相机欧拉角
float yaw_ = 90.0, pitch_ = -20;
// 相邻两帧的时间间隔 用于平衡不同性能机器渲染时相机的移动速度
float sensitivity = 0.05;
Camera camera(cameraPos, yaw_, pitch_, cameraSpeed, sensitivity, fov);

// 全局变量定义的同时必须初始化
float deltaTime = 0, lastTime = 0;
float lastX, lastY;
// 是否为第一帧
bool firstMouse = true;

// 视窗大小改变回调函数
void framebuffer_size_callback(GLFWwindow *window, int width, int height)
{
    if (window)
    {
        glViewport(0, 0, width, height);
    }
}

// 鼠标移动回调函数
void mouse_callback(GLFWwindow *window, double xpos, double ypos)
{
    if (firstMouse)
    {
        lastX = xpos;
        lastY = ypos;
        firstMouse = false;
    }
    camera.turn(lastX, lastY, xpos, ypos);
    lastX = xpos;
    lastY = ypos;
}

// 鼠标滚轮回调函数
void scroll_back(GLFWwindow *window, double xoffset, double yoffset)
{
    camera.zoom(yoffset);
}
void processInput(GLFWwindow *window)
{
    // 关闭窗口
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
    {
        glfwSetWindowShouldClose(window, true);
    }
    camera.move(window, deltaTime);
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
    // 禁用鼠标
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    glfwMakeContextCurrent(window);
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        cout << "GLAD初始化失败!" << endl;
        glfwTerminate(); // 释放资源
        exit(-1);
    }
    glViewport(0, 0, screenWidth, screenHeight);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetScrollCallback(window, scroll_back);
    // 立方体贴图加载函数
    auto loadCubeMap = [](const vector<string> imageFiles, GLenum TextureUnit)
    {
        unsigned int cubMap;
        glGenTextures(1, &cubMap);
        glActiveTexture(TextureUnit);
        glBindTexture(GL_TEXTURE_CUBE_MAP, cubMap);
        // 设置纹理参数
        // 过滤内插方式
        glTextureParameteri(cubMap, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTextureParameteri(cubMap, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        // 纹理环绕方式
        glTextureParameteri(cubMap, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTextureParameteri(cubMap, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTextureParameteri(cubMap, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
        int targetCubMapFace = GL_TEXTURE_CUBE_MAP_POSITIVE_X;
        for (auto &imageFile : imageFiles)
        {
            int width, height, nChannels;
            unsigned char *imgData = nullptr;
            imgData = stbi_load(imageFile.c_str(), &width, &height, &nChannels, 0);
            GLenum format;
            if (nChannels == 1)
            {
                format = GL_RED;
            }
            else if (nChannels == 3)
            {
                format = GL_RGB;
            }
            else if (nChannels == 4)
            {
                format = GL_RGBA;
            }
            if (imgData)
            {
                glTexImage2D(targetCubMapFace, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, imgData);
                stbi_image_free(imgData);
                targetCubMapFace++;
            }
            else
            {
                cout << imageFile << "纹理加载失败!" << endl;
            }
        }
        return cubMap;
    };
    vector<string> imageFiles;
    imageFiles.push_back("../textures/skybox/right.jpg");
    imageFiles.push_back("../textures/skybox/left.jpg");
    imageFiles.push_back("../textures/skybox/top.jpg");
    imageFiles.push_back("../textures/skybox/bottom.jpg");
    imageFiles.push_back("../textures/skybox/back.jpg");
    imageFiles.push_back("../textures/skybox/front.jpg");
    unsigned int cubeMap = loadCubeMap(imageFiles, GL_TEXTURE2);
    // 绘制天空盒
    float skyboxVertices[] = {
        -0.5f, -0.5f, -0.5f, 
        0.5f, -0.5f, -0.5f, 
        0.5f, 0.5f, -0.5f,
        0.5f, 0.5f, -0.5f, 
        -0.5f, 0.5f, -0.5f, 
        -0.5f, -0.5f, -0.5f,

        -0.5f, -0.5f, 0.5f, 
        0.5f, -0.5f, 0.5f, 
        0.5f, 0.5f, 0.5f,
        0.5f, 0.5f, 0.5f,
        -0.5f, 0.5f, 0.5f, 
        -0.5f, -0.5f, 0.5f,

        -0.5f, 0.5f, 0.5f, 
        -0.5f, 0.5f, -0.5f, 
        -0.5f, -0.5f, -0.5f,
        -0.5f, -0.5f, -0.5f,
        -0.5f, -0.5f, 0.5f, 
        -0.5f, 0.5f, 0.5f, 

        0.5f, 0.5f, 0.5f, 
        0.5f, 0.5f, -0.5f, 
        0.5f, -0.5f, -0.5f,
        0.5f, -0.5f, -0.5f, 
        0.5f, -0.5f, 0.5f,
        0.5f, 0.5f, 0.5f,

        -0.5f, -0.5f, -0.5f,
        0.5f, -0.5f, -0.5f, 
        0.5f, -0.5f, 0.5f, 
        0.5f, -0.5f, 0.5f, 
        -0.5f, -0.5f, 0.5f,
        -0.5f, -0.5f, -0.5f, 

        -0.5f, 0.5f, -0.5f,
        0.5f, 0.5f, -0.5f, 
        0.5f, 0.5f, 0.5f, 
        0.5f, 0.5f, 0.5f, 
        -0.5f, 0.5f, 0.5f,
        -0.5f, 0.5f, -0.5f};
    unsigned int sbVao;
    glGenVertexArrays(1, &sbVao);
    glBindVertexArray(sbVao);
    unsigned int sbVbo;
    glGenBuffers(1, &sbVbo);
    glBindBuffer(GL_ARRAY_BUFFER, sbVbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(skyboxVertices), skyboxVertices, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GL_FLOAT), 0);
    glBindVertexArray(0);
    // 天空盒着色器
    Shader sbShader("D:/LearnOpenGL/shader/skybox.vert", "D:/LearnOpenGL/shader/skybox.frag");
    // 开启深度测试
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);
    while (!glfwWindowShouldClose(window))
    {
        processInput(window);
        glClearColor(0.0, 0.0, 0.0, 1.0);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        // 时间参数
        float currentTime = glfwGetTime();
        deltaTime = currentTime - lastTime;
        lastTime = currentTime;
        mat4 view, project;
        view = lookAt(camera.m_cameraPos, camera.m_cameraPos + camera.m_cameraFront, camera.m_cameraUp);
        project = perspective(camera.m_fov, (float)screenWidth / screenHeight, 0.1f, 100.0f);
        // 渲染天空盒 此时禁用深度写入保证天空盒在后续所有渲染的对象的后面
        glBindVertexArray(sbVao);
        // glDepthMask(GL_FALSE);
        sbShader.use();
        sbShader.setUniformMatrix4("view", view);
        sbShader.setUniformMatrix4("project", project);
        // sbShader.setUniformInt("textureUnit", 2);
        glDrawArrays(GL_TRIANGLES, 0, 36);
        // glDepthMask(GL_TRUE);
        glfwSwapBuffers(window);
        glfwPollEvents();
    }
    glBindVertexArray(0);
    glfwTerminate();
    return 0;
}

unsigned int loadTexture(const char *imageFile, GLenum TextureUnit)
{
    // 纹理
    std::cout << imageFile << std::endl;
    unsigned int textureId;
    glGenTextures(1, &textureId);
    // 设置纹理环绕方式
    glTextureParameteri(textureId, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTextureParameteri(textureId, GL_TEXTURE_WRAP_T, GL_REPEAT);
    // 设置纹理过滤方式
    glTextureParameteri(textureId, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTextureParameteri(textureId, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    // 纹理贴图
    int width, height, nChannels;
    unsigned char *textureData = stbi_load(imageFile, &width, &height, &nChannels, 0);
    if (textureData)
    {
        GLenum format;
        if (nChannels == 1)
        {
            format = GL_RED;
        }
        else if (nChannels == 3)
        {
            format = GL_RGB;
        }
        else if (nChannels == 4)
        {
            format = GL_RGBA;
        }
        // 绑定纹理
        glActiveTexture(TextureUnit);
        glBindTexture(GL_TEXTURE_2D, textureId);
        glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, textureData);
        glGenerateMipmap(GL_TEXTURE_2D);
        // 释放贴图内存
        stbi_image_free(textureData);
    }
    else
    {
        std::cout << imageFile << std::endl;
        std::cout << "该纹理贴图载入失败!" << std::endl;
        stbi_image_free(textureData);
    }
    return textureId;
};