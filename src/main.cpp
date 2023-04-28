#include <iostream>
#include <fstream>
#include <sstream>
#include <string.h>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
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
    GLFWwindow *window = glfwCreateWindow(800, 600, "LearnOpenGL", NULL, NULL);
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
    glViewport(0, 0, 800, 600);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    // 顶点着色器
    ifstream vertexIfs;
    vertexIfs.open("D:/LearnOpenGL/shader/vertex.vert");
    stringstream vertexSS;
    vertexSS << vertexIfs.rdbuf();
    vertexIfs.close();
    string vertexCode = vertexSS.str();
    cout << vertexCode << endl;
    cout << vertexCode.length() << endl;
    const char *vertexCode_ = vertexCode.c_str();
    unsigned int vertexShader;
    vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &vertexCode_, nullptr);
    glCompileShader(vertexShader);
    int flag;
    char infoLog[1024];
    glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &flag);
    if (!flag)
    {
        cout << "顶点着色器编译失败!" << endl;
        glGetShaderInfoLog(vertexShader, 1024, nullptr, infoLog);
        cout << infoLog << endl;
        glfwTerminate();
        exit(-1);
    }
    // 片段着色器
    unsigned int fragShader;
    fragShader = glCreateShader(GL_FRAGMENT_SHADER);
    ifstream fragIfs;
    fragIfs.open("D:/LearnOpenGL/shader/frag.frag");
    stringstream fragSS;
    fragSS << fragIfs.rdbuf();
    string fragCode = fragSS.str();
    const char *fragCode_ = fragCode.c_str();
    glShaderSource(fragShader, 1, &fragCode_, nullptr);
    glCompileShader(fragShader);
    glGetShaderiv(fragShader, GL_COMPILE_STATUS, &flag);
    if (!flag)
    {
        cout << "片段着色器编译失败!" << endl;
        glGetShaderInfoLog(fragShader, 1024, nullptr, infoLog);
        cout << infoLog << endl;
        glfwTerminate();
        exit(-1);
    }
    // 链接着色器 生成着色器程序
    unsigned int shaderProgram;
    shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragShader);
    glLinkProgram(shaderProgram);
    glGetProgramiv(shaderProgram, GL_LINK_STATUS, &flag);
    if (!flag)
    {
        cout << "着色器链接失败!" << endl;
        glGetProgramInfoLog(shaderProgram, 1024, nullptr, infoLog);
        cout << infoLog << endl;
        exit(-1);
    }
    glDeleteShader(vertexShader);
    glDeleteShader(fragShader);
    // 顶点数组对象
    unsigned int vao;
    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);
    unsigned int vbo;
    glGenBuffers(1, &vbo);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    float vertexes[] = {-0.5, -0.5, 0.0, 0.5, -0.5, 0.0, 0.0, 0.5, 0.0};
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertexes), vertexes, GL_STATIC_DRAW);
    // 链接着色器顶点属性与顶点数据的对应关系
    glVertexAttribPointer(0, 3, GL_FLOAT, false, 3 * sizeof(GL_FLOAT), 0);
    glEnableVertexAttribArray(0);
    glBindVertexArray(0);
    while (!glfwWindowShouldClose(window))
    {
        processInput(window);
        glClearColor(1.0, 1.0, 1.0, 1.0);
        glClear(GL_COLOR_BUFFER_BIT);
        glUseProgram(shaderProgram);
        glBindVertexArray(vao);
        glDrawArrays(GL_TRIANGLES, 0, 3);
        glfwSwapBuffers(window);
        glfwPollEvents();
    }
    glDeleteVertexArrays(1, &vao);
    glDeleteBuffers(1, &vbo);
    glDeleteProgram(shaderProgram);
    glfwTerminate();
    return 0;
}