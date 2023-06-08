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
// ���ι�Դ�İ뾶��С
#define RAIDUS 1
#define PI 3.1415926
// ���Դ�ĸ���
#define NR_POINT_LIGHTS 4

// �������������ص�ȫ�ֱ���
vec3 cameraPos = vec3(0, 0, 3);
float cameraSpeed = 5;
// �ӳ���С
// ��С������Ŵ��Ч�� ���������С��Ч��
float fov = 45;
// ���ŷ����
float yaw_ = 90.0, pitch_ = -20;
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
    // ��������ͼ���غ���
    auto loadCubeMap = [](const vector<string> imageFiles, GLenum TextureUnit)
    {
        unsigned int cubMap;
        glGenTextures(1, &cubMap);
        glActiveTexture(TextureUnit);
        glBindTexture(GL_TEXTURE_CUBE_MAP, cubMap);
        // �����������
        // �����ڲ巽ʽ
        glTextureParameteri(cubMap, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTextureParameteri(cubMap, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        // �����Ʒ�ʽ
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
                cout << imageFile << "�������ʧ��!" << endl;
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
    // ������պ�
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
    // ��պ���ɫ��
    Shader sbShader("D:/LearnOpenGL/shader/skybox.vert", "D:/LearnOpenGL/shader/skybox.frag");
    // ������Ȳ���
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);
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
        // ��Ⱦ��պ� ��ʱ�������д�뱣֤��պ��ں���������Ⱦ�Ķ���ĺ���
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
    // ����
    std::cout << imageFile << std::endl;
    unsigned int textureId;
    glGenTextures(1, &textureId);
    // ���������Ʒ�ʽ
    glTextureParameteri(textureId, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTextureParameteri(textureId, GL_TEXTURE_WRAP_T, GL_REPEAT);
    // ����������˷�ʽ
    glTextureParameteri(textureId, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTextureParameteri(textureId, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    // ������ͼ
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
        // ������
        glActiveTexture(TextureUnit);
        glBindTexture(GL_TEXTURE_2D, textureId);
        glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, textureData);
        glGenerateMipmap(GL_TEXTURE_2D);
        // �ͷ���ͼ�ڴ�
        stbi_image_free(textureData);
    }
    else
    {
        std::cout << imageFile << std::endl;
        std::cout << "��������ͼ����ʧ��!" << std::endl;
        stbi_image_free(textureData);
    }
    return textureId;
};