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
#include <Sphere.h>
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
float cameraSpeed = 10;
// 视场大小
// 变小会产生放大的效果 变大会产生缩小的效果
float fov = 45;
// 相机欧拉角
float yaw_ = 90.0, pitch_ = 0.0;
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
    // 模型着色器
    Shader shader("../shader/model.vert", "../shader/model.frag","");
    cout << "着色器初始化!" << endl;
    // 载入模型
    Model myModel("D:/LearnOpenGL/Model/nanosuit/nanosuit.obj");
    // 绘制点光源
    Sphere pointLightSphere(X_SEGMENTS, Y_SEGMENTS, RAIDUS);
    // 四个点光源的位置
    vec3 pointlightPositions[] = {vec3(3, 0, 3), vec3(-3, 0, -3), vec3(3, 12, -3), vec3(-3, 12, 3)};
    // 光源着色器
    Shader lightShader("D:/LearnOpenGL/shader/light.vert", "D:/LearnOpenGL/shader/light.frag");
    // 开启深度测试
    glEnable(GL_DEPTH_TEST);

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
        // 绘制点光源
        lightShader.use();
        lightShader.setUniformMatrix4("view", view);
        lightShader.setUniformMatrix4("project", project);
        for (int i = 0; i < NR_POINT_LIGHTS; i++)
        {
            mat4 pointLightModel;
            pointLightModel = translate(pointLightModel, pointlightPositions[i]);
            lightShader.setUniformMatrix4("model", pointLightModel);
            pointLightSphere.Draw(lightShader);
        }
        mat4 model;
        model = scale(model, vec3(0.1));
        shader.use();
        shader.setUniformMatrix4("model", model);
        shader.setUniformMatrix4("view", view);
        shader.setUniformMatrix4("project", project);
        // 设置光源的相关属性
        /****初始化聚束光源******/
        PoiLight bunchedLight;
        bunchedLight.position = camera.m_cameraPos;
        bunchedLight.specular = vec3(0.5, 0.5, 0.5);
        bunchedLight.diffuse = vec3(1.0, 1.0, 1.0);
        bunchedLight.ambient = vec3(0.002, 0.002, 0.002);
        // 设置光源随距离的衰减系数
        bunchedLight.constant = 1.0;
        bunchedLight.linear = 0.09;
        bunchedLight.quadratic = 0.032;
        // 聚光：光源位置、方向、光切角、外广切角
        bunchedLight.direction = camera.m_cameraFront;
        bunchedLight.cutOff = cos(radians(10.0));
        bunchedLight.outerCutOff = cos(radians(12.0));
        shader.setUniformPoiLight("bunchedLight", bunchedLight);
        /****初始化定向光源******/
        DirLight dirLight;
        dirLight.direction = vec3(-0.2, -1.0, -0.3);
        dirLight.ambient = vec3(0.005, 0.005, 0.005);
        dirLight.diffuse = vec3(0.4, 0.4, 0.4);
        dirLight.specular = vec3(0.5, 0.5, 0.5);
        shader.setUniformDirLight("dirLight", dirLight);
        /****初始化点光源******/
        for (int i = 0; i < NR_POINT_LIGHTS; i++)
        {
            PoiLight poiLight;
            poiLight.position = pointlightPositions[i];
            poiLight.ambient = vec3(0.005, 0.005, 0.005);
            poiLight.diffuse = vec3(0.8, 0.8, 0.8);
            poiLight.specular = vec3(1.0, 1.0, 1.0);
            poiLight.constant = 1.0;
            poiLight.linear = 0.09;
            poiLight.quadratic = 0.032;
            char uniformName[1024];
            sprintf(uniformName, "poiLights[%d]", i);
            shader.setUniformPoiLight(uniformName, poiLight);
        }
        // 设置相机的位置
        shader.setUniformVec3("cameraPos", cameraPos);
        // 法线矩阵
        shader.setUniformMatrix3("normalMatrix", transpose(inverse(mat3(model))));
        // 设置贴图以外材质参数
        // 环境光反射率
        shader.setUniformVec3("material.ambient", vec3(0.5, 0.5, 0.5));
        // 反光度
        shader.setUniformInt("material.shininess", 3);
        myModel.Draw(shader);
        glfwSwapBuffers(window);
        glfwPollEvents();
    }
    shader.destroy();
    glfwTerminate();
    return 0;
}