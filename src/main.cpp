#include <iostream>
#include <fstream>
#include <sstream>
#include <string.h>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <math.h>
#include <ctime>
#include <Shader.h>

// 引入标准图像库 加载纹理贴图
#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

using namespace std;

void framebuffer_size_callback(GLFWwindow *window, int width, int height)
{
    glViewport(0, 0, width, height);
}

void processInput(GLFWwindow *window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
    {
        glfwSetWindowShouldClose(window, true);
    }
}

int main()
{

    // 创建窗口
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    GLFWwindow *window = glfwCreateWindow(800, 800, "LearnOpenGL", NULL, NULL);
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
    glViewport(0, 0, 800, 800);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    Shader shader("../shader/vertex.vert", "../shader/frag.frag");
    // 顶点数组对象
    float vertices[] = {
        -1.0, -1.0, 0, 1, 0, 0, 0, 1,
        1.0, -1.0, 0, 0, 1, 0, 1, 1,
        1.0, 1.0, 0, 0, 0, 1, 1, 0,
        -1.0, 1.0, 0, 1, 1, 0, 0, 0};
    unsigned int indices[] = {
        0, 1, 2,
        0, 2, 3};
    unsigned int vao;
    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);
    unsigned int vbo;
    glGenBuffers(1, &vbo);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    // 元素缓冲对象
    unsigned int ibo;
    glGenBuffers(1, &ibo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);
    // 链接着色器顶点属性与顶点数据的对应关系
    glVertexAttribPointer(0, 3, GL_FLOAT, false, 8 * sizeof(GL_FLOAT), 0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, false, 8 * sizeof(GL_FLOAT), (void *)(3 * sizeof(GL_FLOAT)));
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(2, 2, GL_FLOAT, false, 8 * sizeof(GL_FLOAT), (void *)(6 * sizeof(GL_FLOAT)));
    glEnableVertexAttribArray(2);
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
            cout << imageFile << endl;
        }
        else
        {
            cout << imageFile << endl;
            cout << "该纹理贴图载入失败!" << endl;
        }
    };
    // 纹理
    unsigned int texture0;
    loadTexture(texture0, "D:/LearnOpenGL/src/texture0.png", GL_TEXTURE0);
    unsigned int texture1;
    loadTexture(texture1, "D:/LearnOpenGL/src/texture1.png", GL_TEXTURE1);
    // 设置着色器sampler 一定要先激活着色器程序
    shader.use();
    shader.setUniformInt("texture0", 0);
    shader.setUniformInt("texture1", 1);
    while (!glfwWindowShouldClose(window))
    {
        processInput(window);
        glClearColor(1.0, 1.0, 1.0, 1.0);
        glClear(GL_COLOR_BUFFER_BIT);
        shader.use();
        float timeValue = clock() % (CLOCKS_PER_SEC * 5);
        timeValue /= CLOCKS_PER_SEC;
        float mixPercent = timeValue / 5;
        shader.setUniformFloat("mixPercent", mixPercent);
        glBindVertexArray(vao);
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
        glfwSwapBuffers(window);
        glfwPollEvents();
    }
    glDeleteVertexArrays(1, &vao);
    glDeleteBuffers(1, &vbo);
    glDeleteBuffers(1, &ibo);
    shader.destroy();
    glDeleteTextures(1, &texture0);
    glDeleteTextures(1, &texture1);
    glfwTerminate();
    return 0;
}