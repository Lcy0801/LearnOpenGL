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
// ���ι�Դ�İ뾶��С
#define RAIDUS 1
#define PI 3.1415926
// ���Դ�ĸ���
#define NR_POINT_LIGHTS 4

// �������������ص�ȫ�ֱ���
vec3 cameraPos = vec3(0, 0, 3);
float cameraSpeed = 10;
// �ӳ���С
// ��С������Ŵ��Ч�� ���������С��Ч��
float fov = 45;
// ���ŷ����
float yaw_ = 90.0, pitch_ = 0.0;
// ������֡��ʱ���� ����ƽ�ⲻͬ���ܻ�����Ⱦʱ������ƶ��ٶ�
float sensitivity = 0.05;
Camera camera(cameraPos, yaw_, pitch_, cameraSpeed, sensitivity, fov);

// ȫ�ֱ��������ͬʱ�����ʼ��
float deltaTime = 0, lastTime = 0;
float lastX, lastY;
// �Ƿ�Ϊ��һ֡
bool firstMouse = true;

// �Ӵ���С�ı�ص�����
void framebuffer_size_callback(GLFWwindow *window, int width, int height)
{
    if (window)
    {
        glViewport(0, 0, width, height);
    }
}

// ����ƶ��ص�����
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

// �����ֻص�����
void scroll_back(GLFWwindow *window, double xoffset, double yoffset)
{
    camera.zoom(yoffset);
}
void processInput(GLFWwindow *window)
{
    // �رմ���
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
    {
        glfwSetWindowShouldClose(window, true);
    }
    camera.move(window, deltaTime);
}

int main()
{

    // ��������
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    GLFWwindow *window = glfwCreateWindow(screenWidth, screenHeight, "LearnOpenGL", NULL, NULL);
    if (window == NULL)
    {
        cout << "��ʼ������ʧ��!" << endl;
        glfwTerminate();
        exit(-1);
    }
    // �������
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    glfwMakeContextCurrent(window);
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        cout << "GLAD��ʼ��ʧ��!" << endl;
        glfwTerminate(); // �ͷ���Դ
        exit(-1);
    }
    glViewport(0, 0, screenWidth, screenHeight);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetScrollCallback(window, scroll_back);
    // ģ����ɫ��
    Shader shader("../shader/model.vert", "../shader/model.frag","");
    cout << "��ɫ����ʼ��!" << endl;
    // ����ģ��
    Model myModel("D:/LearnOpenGL/Model/nanosuit/nanosuit.obj");
    // ���Ƶ��Դ
    Sphere pointLightSphere(X_SEGMENTS, Y_SEGMENTS, RAIDUS);
    // �ĸ����Դ��λ��
    vec3 pointlightPositions[] = {vec3(3, 0, 3), vec3(-3, 0, -3), vec3(3, 12, -3), vec3(-3, 12, 3)};
    // ��Դ��ɫ��
    Shader lightShader("D:/LearnOpenGL/shader/light.vert", "D:/LearnOpenGL/shader/light.frag");
    // ������Ȳ���
    glEnable(GL_DEPTH_TEST);

    while (!glfwWindowShouldClose(window))
    {
        processInput(window);
        glClearColor(0.0, 0.0, 0.0, 1.0);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        // ʱ�����
        float currentTime = glfwGetTime();
        deltaTime = currentTime - lastTime;
        lastTime = currentTime;
        mat4 view, project;
        view = lookAt(camera.m_cameraPos, camera.m_cameraPos + camera.m_cameraFront, camera.m_cameraUp);
        project = perspective(camera.m_fov, (float)screenWidth / screenHeight, 0.1f, 100.0f);
        // ���Ƶ��Դ
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
        // ���ù�Դ���������
        /****��ʼ��������Դ******/
        PoiLight bunchedLight;
        bunchedLight.position = camera.m_cameraPos;
        bunchedLight.specular = vec3(0.5, 0.5, 0.5);
        bunchedLight.diffuse = vec3(1.0, 1.0, 1.0);
        bunchedLight.ambient = vec3(0.002, 0.002, 0.002);
        // ���ù�Դ������˥��ϵ��
        bunchedLight.constant = 1.0;
        bunchedLight.linear = 0.09;
        bunchedLight.quadratic = 0.032;
        // �۹⣺��Դλ�á����򡢹��нǡ�����н�
        bunchedLight.direction = camera.m_cameraFront;
        bunchedLight.cutOff = cos(radians(10.0));
        bunchedLight.outerCutOff = cos(radians(12.0));
        shader.setUniformPoiLight("bunchedLight", bunchedLight);
        /****��ʼ�������Դ******/
        DirLight dirLight;
        dirLight.direction = vec3(-0.2, -1.0, -0.3);
        dirLight.ambient = vec3(0.005, 0.005, 0.005);
        dirLight.diffuse = vec3(0.4, 0.4, 0.4);
        dirLight.specular = vec3(0.5, 0.5, 0.5);
        shader.setUniformDirLight("dirLight", dirLight);
        /****��ʼ�����Դ******/
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
        // ���������λ��
        shader.setUniformVec3("cameraPos", cameraPos);
        // ���߾���
        shader.setUniformMatrix3("normalMatrix", transpose(inverse(mat3(model))));
        // ������ͼ������ʲ���
        // �����ⷴ����
        shader.setUniformVec3("material.ambient", vec3(0.5, 0.5, 0.5));
        // �����
        shader.setUniformInt("material.shininess", 3);
        myModel.Draw(shader);
        glfwSwapBuffers(window);
        glfwPollEvents();
    }
    shader.destroy();
    glfwTerminate();
    return 0;
}