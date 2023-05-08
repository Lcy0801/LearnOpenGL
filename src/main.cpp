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
#include <vector>
using namespace std;
using namespace glm;

// 引入标准图像库 加载纹理贴图
#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>
#define screenWidth 800
#define screenHeight 800

#define X_SEGMENTS 360
#define Y_SEGMENTS 180
// 球形光源的半径大小
#define RAIDUS 1
#define PI 3.1415926
#define LIGHTPOS vec3(1, 1, 1)

// 光源位置

// 定义相机参数相关的全局变量
vec3 cameraPos = vec3(0, 0, 3);
vec3 cameraFront = vec3(0, 0, -1);
vec3 cameraUp = vec3(0, 1, 0);
float cameraSpeed = 10;
// 视场大小
// 变小会产生放大的效果 变大会产生缩小的效果
float fov = 45;

// 相机欧拉角
// 全局变量定义的同时必须初始化
float yaw_ = 90.0, pitch_ = 0.0;
// 相邻两帧的时间间隔 用于平衡不同性能机器渲染时相机的移动速度
float deltaTime = 0, lastTime = 0;
float lastX = 400, lastY = 400;
float sensitivity = 0.05;
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
    float xoffset, yoffset;
    xoffset = xpos - lastX;
    yoffset = ypos - lastY;
    lastX = xpos;
    lastY = ypos;
    xoffset *= sensitivity;
    yoffset *= sensitivity;
    yaw_ -= xoffset;
    pitch_ -= yoffset;
    if (pitch_ > 89)
    {
        pitch_ = 89;
    }
    else if (pitch_ < -89)
    {
        pitch_ = -89;
    }
    float directionX, directionY, directionZ;
    directionX = cos(radians(pitch_)) * cos(radians(yaw_));
    directionY = sin(radians(pitch_));
    directionZ = -cos(radians(pitch_)) * sin(radians(yaw_));
    cameraFront = normalize(vec3(directionX, directionY, directionZ));
    // 更新cameraUP
    // vec3 cameraRight;
    // cameraRight[0] = sin(radians(yaw_));
    // cameraRight[1] = 0;
    // cameraFront[2] = cos(radians(yaw_));
    // cameraUp = normalize(cross(-cameraFront,cameraRight));
}

// 鼠标滚轮回调函数
void scroll_back(GLFWwindow *window, double xoffset, double yoffset)
{
    fov += sensitivity * yoffset;
    if (fov > 45)
    {
        fov = 45;
    }
    if (fov < 1)
    {
        fov = 1;
    }
    return;
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
    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetScrollCallback(window, scroll_back);
    // 绘制投光物
    Shader shader("../shader/vertex.vert", "../shader/frag.frag");
    float vertices[] = {
        -0.5f, -0.5f, -0.5f, 0.0f, 0.0f, -1.0f, 0.0f, 0.0f,
        0.5f, -0.5f, -0.5f, 0.0f, 0.0f, -1.0f, 1.0f, 0.0f,
        0.5f, 0.5f, -0.5f, 0.0f, 0.0f, -1.0f, 1.0f, 1.0f,
        0.5f, 0.5f, -0.5f, 0.0f, 0.0f, -1.0f, 1.0f, 1.0f,
        -0.5f, 0.5f, -0.5f, 0.0f, 0.0f, -1.0f, 0.0f, 1.0f,
        -0.5f, -0.5f, -0.5f, 0.0f, 0.0f, -1.0f, 0.0f, 0.0f,

        -0.5f, -0.5f, 0.5f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f,
        0.5f, -0.5f, 0.5f, 0.0f, 0.0f, 1.0f, 1.0f, 0.0f,
        0.5f, 0.5f, 0.5f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f,
        0.5f, 0.5f, 0.5f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f,
        -0.5f, 0.5f, 0.5f, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f,
        -0.5f, -0.5f, 0.5f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f,

        -0.5f, 0.5f, 0.5f, -1.0f, 0.0f, 0.0f, 1.0f, 0.0f,
        -0.5f, 0.5f, -0.5f, -1.0f, 0.0f, 0.0f, 1.0f, 1.0f,
        -0.5f, -0.5f, -0.5f, -1.0f, 0.0f, 0.0f, 0.0f, 1.0f,
        -0.5f, -0.5f, -0.5f, -1.0f, 0.0f, 0.0f, 0.0f, 1.0f,
        -0.5f, -0.5f, 0.5f, -1.0f, 0.0f, 0.0f, 0.0f, 0.0f,
        -0.5f, 0.5f, 0.5f, -1.0f, 0.0f, 0.0f, 1.0f, 0.0f,

        0.5f, 0.5f, 0.5f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f,
        0.5f, 0.5f, -0.5f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f,
        0.5f, -0.5f, -0.5f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f,
        0.5f, -0.5f, -0.5f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f,
        0.5f, -0.5f, 0.5f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f,
        0.5f, 0.5f, 0.5f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f,

        -0.5f, -0.5f, -0.5f, 0.0f, -1.0f, 0.0f, 0.0f, 1.0f,
        0.5f, -0.5f, -0.5f, 0.0f, -1.0f, 0.0f, 1.0f, 1.0f,
        0.5f, -0.5f, 0.5f, 0.0f, -1.0f, 0.0f, 1.0f, 0.0f,
        0.5f, -0.5f, 0.5f, 0.0f, -1.0f, 0.0f, 1.0f, 0.0f,
        -0.5f, -0.5f, 0.5f, 0.0f, -1.0f, 0.0f, 0.0f, 0.0f,
        -0.5f, -0.5f, -0.5f, 0.0f, -1.0f, 0.0f, 0.0f, 1.0f,

        -0.5f, 0.5f, -0.5f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f,
        0.5f, 0.5f, -0.5f, 0.0f, 1.0f, 0.0f, 1.0f, 1.0f,
        0.5f, 0.5f, 0.5f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f,
        0.5f, 0.5f, 0.5f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f,
        -0.5f, 0.5f, 0.5f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f,
        -0.5f, 0.5f, -0.5f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f};
    vec3 cubePositions[] = {
        glm::vec3(0.0f, 0.0f, 0.0f),
        glm::vec3(2.0f, 5.0f, -15.0f),
        glm::vec3(-1.5f, -2.2f, -2.5f),
        glm::vec3(-3.8f, -2.0f, -12.3f),
        glm::vec3(2.4f, -0.4f, -3.5f),
        glm::vec3(-1.7f, 3.0f, -7.5f),
        glm::vec3(1.3f, -2.0f, -2.5f),
        glm::vec3(1.5f, 2.0f, -2.5f),
        glm::vec3(1.5f, 0.2f, -1.5f),
        glm::vec3(-1.3f, 1.0f, -1.5f)};
    // 顶点数组对象
    unsigned int vao;
    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);
    unsigned int vbo;
    glGenBuffers(1, &vbo);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    // 链接着色器顶点属性与顶点数据的对应关系
    glVertexAttribPointer(0, 3, GL_FLOAT, false, 8 * sizeof(GL_FLOAT), 0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, false, 8 * sizeof(GL_FLOAT), (void *)(3 * sizeof(GL_FLOAT)));
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(2, 2, GL_FLOAT, false, 8 * sizeof(GL_FLOAT), (void *)(6 * sizeof(GL_FLOAT)));
    glEnableVertexAttribArray(2);
    // 纹理
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
    // 载入漫反射率贴图和镜面反射率贴图
    unsigned int texture0;
    loadTexture(texture0, "D:/LearnOpenGL/src/diffuseMap.png", GL_TEXTURE0);
    unsigned int texture1;
    loadTexture(texture1, "D:/LearnOpenGL/src/specularMap.png", GL_TEXTURE1);
    // 绑定默认vao
    glBindVertexArray(0);
    // 开启深度测试
    glEnable(GL_DEPTH_TEST);
    // 开启剔除面
    // glEnable(GL_CULL_FACE);
    // glCullFace(GL_BACK);
    // 鼠标输入相关操作
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    // 绘制球形光源
    // 计算球面顶点
    vector<float> sphereVertices;
    float dxAngle = 2 * PI / X_SEGMENTS;
    float dyAngle = PI / Y_SEGMENTS;
    for (int i = 0; i <= Y_SEGMENTS; i++)
    {
        float y = cos(i * dyAngle);
        for (int j = 0; j <= X_SEGMENTS; j++)
        {
            float x = sin(i * dyAngle) * cos(j * dxAngle);
            float z = sin(i * dyAngle) * sin(j * dxAngle);
            sphereVertices.push_back(x);
            sphereVertices.push_back(y);
            sphereVertices.push_back(z);
        }
    }
    // 计算球面三角对应的顶点索引
    vector<int> sphereIndices;
    for (int i = 0; i < Y_SEGMENTS; i++)
    {
        for (int j = 0; j < X_SEGMENTS; j++)
        {
            int index1 = i * (X_SEGMENTS + 1) + j;
            int index2 = i * (X_SEGMENTS + 1) + j + 1;
            int index3 = (i + 1) * (X_SEGMENTS + 1) + j;
            int index4 = (i + 1) * (X_SEGMENTS + 1) + j + 1;
            sphereIndices.push_back(index1);
            sphereIndices.push_back(index2);
            sphereIndices.push_back(index4);
            sphereIndices.push_back(index1);
            sphereIndices.push_back(index3);
            sphereIndices.push_back(index4);
        }
    }
    unsigned int lightVao;
    glGenVertexArrays(1, &lightVao);
    glBindVertexArray(lightVao);
    unsigned int lightVbo;
    glGenBuffers(1, &lightVbo);
    glBindBuffer(GL_ARRAY_BUFFER, lightVbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(GL_FLOAT) * sphereVertices.size(), &sphereVertices[0], GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, false, 3 * sizeof(GL_FLOAT), 0);
    glEnableVertexAttribArray(0);
    unsigned int lightIbo;
    glGenBuffers(1, &lightIbo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, lightIbo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GL_INT) * sphereIndices.size(), &sphereIndices[0], GL_STATIC_DRAW);
    glBindVertexArray(0);
    // 光源着色器
    Shader lightShader("D:/LearnOpenGL/shader/light.vert", "D:/LearnOpenGL/shader/light.frag");

    while (!glfwWindowShouldClose(window))
    {
        processInput(window);
        glClearColor(0.0, 0.0, 0.0, 1.0);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        mat4 model, view, project;
        model = translate(model, LIGHTPOS);
        model = scale(model, vec3(0.1));
        view = lookAt(cameraPos, cameraPos + cameraFront, cameraUp);
        project = perspective(fov, (float)screenWidth / screenHeight, 0.1f, 100.0f);
        glBindVertexArray(lightVao);
        lightShader.use();
        lightShader.setUniformMatrix4("model", model);
        lightShader.setUniformMatrix4("view", view);
        lightShader.setUniformMatrix4("project", project);
        glDrawElements(GL_TRIANGLES, sphereIndices.size(), GL_UNSIGNED_INT, 0);
        glBindVertexArray(vao);
        shader.use();
        // 设置光源的相关属性
        float currentTime = glfwGetTime();
        // 更具时间改变光源的颜色
        vec3 lightColor = vec3(sin(currentTime * 2.0), sin(currentTime * 0.7), sin(currentTime * 1.3));
        shader.setUniformVec3("light.lightColor", vec3(1));
        shader.setUniformVec3("light.specular", vec3(0.5, 0.5, 0.5));
        shader.setUniformVec3("light.diffuse", vec3(1.0, 1.0, 1.0));
        shader.setUniformVec3("light.ambient", vec3(0.2, 0.2, 0.2));
        // 聚光：光源位置、方向、光切角、外广切角
        shader.setUniformVec3("light.lightPos", cameraPos);
        shader.setUniformVec3("light.lightDir",cameraFront);
        shader.setUniformFloat("light.cutOff", cos(radians(5.0)));
        shader.setUniformFloat("light.outerCutOff", cos(radians(6.0)));
        // 设置光源随距离的衰减系数
        shader.setUniformFloat("light.constant", 1.0);
        shader.setUniformFloat("light.linear", 0.22);
        shader.setUniformFloat("light.quadratic", 0.20);
        // 设置材质
        shader.setUniformVec3("material.ambient", vec3(1.0, 0.5, 0.31));
        // shader.setUniformVec3("material.diffuse", vec3(1.0f, 0.5f, 0.31f));
        // 漫反射贴图
        shader.setUniformInt("material.diffuseMap", 0);
        // shader.setUniformVec3("material.specular", vec3(0.5f, 0.5f, 0.5f));
        // 镜面反射贴图
        shader.setUniformInt("material.specularMap", 1);
        // 反光度:镜面反射的衰减系数
        shader.setUniformInt("material.shininess", 9);
        // 设置相机的位置
        shader.setUniformVec3("cameraPos", cameraPos);
        for (int i = 0; i < 10;i++)
        {
            mat4 model_;
            model_ = translate(model_, cubePositions[i]);
            model_ = rotate(model_, currentTime, vec3(1, 1, 1));
            shader.setUniformMatrix4("model", model_);
            shader.setUniformMatrix4("view", view);
            shader.setUniformMatrix4("project", project);
            // 设置片段着色器中的法线矩阵
            shader.setUniformMatrix3("normalMatrix", transpose(inverse(mat3(model_))));
            glDrawArrays(GL_TRIANGLES, 0, 36);
        }
        glfwSwapBuffers(window);
        deltaTime = currentTime - lastTime;
        lastTime = currentTime;
        glfwPollEvents();
    }
    glDeleteVertexArrays(1, &vao);
    glDeleteBuffers(1, &vbo);
    shader.destroy();
    glfwTerminate();
    return 0;
}