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

// �����׼ͼ��� ����������ͼ
// #define STB_IMAGE_IMPLEMENTATION
// #include <stb_image.h>
#define screenWidth 800
#define screenHeight 800

#define X_SEGMENTS 360
#define Y_SEGMENTS 180
// ���ι�Դ�İ뾶��С
#define RAIDUS 1
#define PI 3.1415926
// ���Դ�ĸ���
#define NR_POINT_LIGHTS 4

// ��Դλ��

// �������������ص�ȫ�ֱ���
vec3 cameraPos = vec3(0, 10, 15);
vec3 cameraFront = vec3(0, 0, -1);
vec3 cameraUp = vec3(0, 1, 0);
float cameraSpeed = 10;
// �ӳ���С
// ��С������Ŵ��Ч�� ���������С��Ч��
float fov = 45;

// ���ŷ����
// ȫ�ֱ��������ͬʱ�����ʼ��
float yaw_ = 90.0, pitch_ = 0.0;
// ������֡��ʱ���� ����ƽ�ⲻͬ���ܻ�����Ⱦʱ������ƶ��ٶ�
float deltaTime = 0, lastTime = 0;
float lastX = 400, lastY = 400;
float sensitivity = 0.05;
bool firstMouse = true;
// amera(glm::vec3 cameraPos, float yaw_, float pitch_, float speed, float sensitivity, float fov)
Camera camera(cameraPos,yaw_,pitch_,cameraSpeed,sensitivity,fov);

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
    // ����cameraUP
    // vec3 cameraRight;
    // cameraRight[0] = sin(radians(yaw_));
    // cameraRight[1] = 0;
    // cameraFront[2] = cos(radians(yaw_));
    // cameraUp = normalize(cross(-cameraFront,cameraRight));
}

// �����ֻص�����
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
    // �رմ���
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
    {
        glfwSetWindowShouldClose(window, true);
    }
    // ǰ��
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
    {
        cameraPos = cameraPos + cameraFront * cameraSpeed * deltaTime;
    }
    // ����
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
    {
        cameraPos = cameraPos - cameraFront * cameraSpeed * deltaTime;
    }
    // ����
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
    {
        cameraPos = cameraPos + normalize(cross(cameraUp, cameraFront)) * cameraSpeed * deltaTime;
    }
    // ����
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
    {
        cameraPos = cameraPos + normalize(cross(cameraFront, cameraUp)) * cameraSpeed * deltaTime;
    }
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
    Shader shader("../shader/model.vert", "../shader/model.frag");
    cout << "��ɫ����ʼ��!" << endl;
    // ����ģ��
    Model myModel("D:/LearnOpenGL/Model/nanosuit/nanosuit.obj");
    // ������Ȳ���
    glEnable(GL_DEPTH_TEST);
    // ���������ز���
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    // ���Ƶ��Դ
    // �ĸ����Դ��λ��
    // vec3 pointlightPositions[] = {
    //     vec3(0.7, 0.2, 2.0),
    //     vec3(2.3, -3.3, -4.0),
    //     vec3(-4.0, 2.0, -12.0),
    //     vec3(0.0, 0.0, -3.0)};
    // // �������涥��
    // vector<float> sphereVertices;
    // float dxAngle = 2 * PI / X_SEGMENTS;
    // float dyAngle = PI / Y_SEGMENTS;
    // for (int i = 0; i <= Y_SEGMENTS; i++)
    // {
    //     float y = cos(i * dyAngle);
    //     for (int j = 0; j <= X_SEGMENTS; j++)
    //     {
    //         float x = sin(i * dyAngle) * cos(j * dxAngle);
    //         float z = sin(i * dyAngle) * sin(j * dxAngle);
    //         sphereVertices.push_back(x);
    //         sphereVertices.push_back(y);
    //         sphereVertices.push_back(z);
    //     }
    // }
    // // �����������Ƕ�Ӧ�Ķ�������
    // vector<int> sphereIndices;
    // for (int i = 0; i < Y_SEGMENTS; i++)
    // {
    //     for (int j = 0; j < X_SEGMENTS; j++)
    //     {
    //         int index1 = i * (X_SEGMENTS + 1) + j;
    //         int index2 = i * (X_SEGMENTS + 1) + j + 1;
    //         int index3 = (i + 1) * (X_SEGMENTS + 1) + j;
    //         int index4 = (i + 1) * (X_SEGMENTS + 1) + j + 1;
    //         sphereIndices.push_back(index1);
    //         sphereIndices.push_back(index2);
    //         sphereIndices.push_back(index4);
    //         sphereIndices.push_back(index1);
    //         sphereIndices.push_back(index3);
    //         sphereIndices.push_back(index4);
    //     }
    // }
    // unsigned int lightVao;
    // glGenVertexArrays(1, &lightVao);
    // glBindVertexArray(lightVao);
    // unsigned int lightVbo;
    // glGenBuffers(1, &lightVbo);
    // glBindBuffer(GL_ARRAY_BUFFER, lightVbo);
    // glBufferData(GL_ARRAY_BUFFER, sizeof(GL_FLOAT) * sphereVertices.size(), &sphereVertices[0], GL_STATIC_DRAW);
    // glVertexAttribPointer(0, 3, GL_FLOAT, false, 3 * sizeof(GL_FLOAT), 0);
    // glEnableVertexAttribArray(0);
    // unsigned int lightIbo;
    // glGenBuffers(1, &lightIbo);
    // glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, lightIbo);
    // glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GL_INT) * sphereIndices.size(), &sphereIndices[0], GL_STATIC_DRAW);
    // glBindVertexArray(0);
    // // ��Դ��ɫ��
    // Shader lightShader("D:/LearnOpenGL/shader/light.vert", "D:/LearnOpenGL/shader/light.frag");

    while (!glfwWindowShouldClose(window))
    {
        processInput(window);
        glClearColor(0.0, 0.0, 0.0, 1.0);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        mat4 view, project, model;
        model = translate(model, vec3(0, 0, 0));
        view = lookAt(cameraPos, cameraPos + cameraFront, cameraUp);
        project = perspective(fov, (float)screenWidth / screenHeight, 0.1f, 100.0f);
        shader.use();
        shader.setUniformMatrix4("model", model);
        shader.setUniformMatrix4("view", view);
        shader.setUniformMatrix4("project", project);
        // lightShader.use();
        // glBindVertexArray(lightVao);
        // // ���Ƶ��Դ
        // for (int i = 0; i < 4; i++)
        // {
        //     mat4 model;
        //     model = translate(model, pointlightPositions[i]);
        //     model = scale(model, vec3(0.1));
        //     lightShader.setUniformMatrix4("model", model);
        //     lightShader.setUniformMatrix4("view", view);
        //     lightShader.setUniformMatrix4("project", project);
        //     glDrawElements(GL_TRIANGLES, sphereIndices.size(), GL_UNSIGNED_INT, 0);
        // }
        // ����ģ��
        // glBindVertexArray(vao);
        myModel.Draw(shader);
        // // ���ù�Դ���������
        float currentTime = glfwGetTime();
        // /****��ʼ��������Դ******/
        // PoiLight bunchedLight;
        // bunchedLight.position = cameraPos;
        // bunchedLight.specular = vec3(0.5, 0.5, 0.5);
        // bunchedLight.diffuse = vec3(1.0, 1.0, 1.0);
        // bunchedLight.ambient = vec3(0.02, 0.02, 0.02);
        // // ���ù�Դ������˥��ϵ��
        // bunchedLight.constant = 1.0;
        // bunchedLight.linear = 0.22;
        // bunchedLight.quadratic = 0.20;
        // // �۹⣺��Դλ�á����򡢹��нǡ�����н�
        // bunchedLight.direction = cameraFront;
        // bunchedLight.cutOff = cos(radians(10.0));
        // bunchedLight.outerCutOff = cos(radians(12.0));
        // shader.setUniformPoiLight("bunchedLight", bunchedLight);
        /****��ʼ�������Դ******/
        // DirLight dirLight;
        // dirLight.direction = vec3(-0.2, -1.0, -0.3);
        // dirLight.ambient = vec3(0.005, 0.005, 0.005);
        // dirLight.diffuse = vec3(0.4, 0.4, 0.4);
        // dirLight.specular = vec3(0.5, 0.5, 0.5);
        // shader.setUniformDirLight("dirLight", dirLight);
        // /****��ʼ�����Դ******/
        // for (int i = 0; i < NR_POINT_LIGHTS; i++)
        // {
        //     PoiLight poiLight;
        //     poiLight.position = pointlightPositions[i];
        //     poiLight.ambient = vec3(0.005, 0.005, 0.005);
        //     poiLight.diffuse = vec3(0.8, 0.8, 0.8);
        //     poiLight.specular = vec3(1.0, 1.0, 1.0);
        //     poiLight.constant = 1.0;
        //     poiLight.linear = 0.09;
        //     poiLight.quadratic = 0.032;
        //     char uniformName[1024];
        //     sprintf(uniformName, "poiLights[%d]", i);
        //     shader.setUniformPoiLight(uniformName, poiLight);
        // }
        // ���ò���
        // Material material;
        // material.ambient = vec3(1.0, 0.5, 0.31);
        // // ��������ͼ
        // material.diffuseMap = 0;
        // // ���淴����ͼ
        // material.specularMap = 1;
        // // �����:���淴���˥��ϵ��
        // material.shininess = 3;
        // shader.setUniformMaterail("material", material);
        // ���������λ��
        // shader.setUniformVec3("cameraPos", cameraPos);
        // float mixPercent = sin(currentTime);
        // shader.setUniformFloat("mixPercent", mixPercent);
        // for (int i = 0; i < 10; i++)
        // {
        //     mat4 model_;
        //     model_ = translate(model_, cubePositions[i]);
        //     model_ = rotate(model_, currentTime + radians(i * 10.0f), vec3(1, 1, 1));
        //     shader.setUniformMatrix4("model", model_);
        //     shader.setUniformMatrix4("view", view);
        //     shader.setUniformMatrix4("project", project);
        //     // ����Ƭ����ɫ���еķ��߾���
        //     shader.setUniformMatrix3("normalMatrix", transpose(inverse(mat3(model_))));
        //     for (int j = 0; j < 6; j++)
        //     {
        //         shader.setUniformInt("texture0", 2 * (j + 1));
        //         shader.setUniformInt("texture1", 2 * (j + 1) + 1);
        //         glDrawArrays(GL_TRIANGLES, j * 6, 6);
        //     }
        // }
        glfwSwapBuffers(window);
        deltaTime = currentTime - lastTime;
        lastTime = currentTime;
        glfwPollEvents();
    }
    shader.destroy();
    glfwTerminate();
    return 0;
}