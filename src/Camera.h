
// OpenGL矩阵运算库
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

class Camera
{
public:
    // 相机位置
    glm::vec3 m_cameraPos;
    // 相机z轴
    glm::vec3 m_cameraFront;
    // 相机up轴
    glm::vec3 m_cameraUp;
    // 相机姿态欧拉角
    float m_yaw;
    float m_pitch;
    // 相机移动速度
    float m_cameraSpeed;
    float m_sensitivity;
    // 相机视场角
    float m_fov;
    Camera(glm::vec3 cameraPos, float yaw_, float pitch_, float speed, float sensitivity, float fov)
    {
        m_cameraPos = cameraPos;
        m_yaw = yaw_;
        m_pitch = pitch_;
        // 基于欧拉角计算相机观察方向向量
        float directionX, directionY, directionZ;
        directionX = std::cos(glm::radians(pitch_)) * std::cos(glm::radians(yaw_));
        directionY = std::sin(glm::radians(pitch_));
        directionZ = -std::cos(glm::radians(pitch_)) * std::sin(glm::radians(yaw_));
        m_cameraFront = glm::normalize(glm::vec3(directionX, directionY, directionZ));
        m_cameraUp = glm::vec3(0, 1, 0);
        m_cameraSpeed = speed;
        m_sensitivity = sensitivity;
        m_fov = fov;
    }
    // 相机移动
    void move(GLFWwindow *window, float deltaTime)
    {
        // 前进
        if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        {
            m_cameraPos = m_cameraPos + m_cameraFront * m_cameraSpeed * deltaTime;
        }
        // 后退
        if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        {
            m_cameraPos = m_cameraPos - m_cameraFront * m_cameraSpeed * deltaTime;
        }
        // 左移
        if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        {
            m_cameraPos = m_cameraPos + normalize(cross(m_cameraUp, m_cameraFront)) * m_cameraSpeed * deltaTime;
        }
        // 右移
        if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        {
            m_cameraPos = m_cameraPos + normalize(cross(m_cameraFront, m_cameraUp)) * m_cameraSpeed * deltaTime;
        }
    }
    // 镜头转向
    void turn(double lastX, double lastY, double xpos, double ypos)
    {
        float xoffset, yoffset;
        xoffset = xpos - lastX;
        yoffset = ypos - lastY;
        xoffset *= m_sensitivity;
        yoffset *= m_sensitivity;
        m_yaw -= xoffset;
        m_pitch -= yoffset;
        if (m_pitch > 89)
        {
            m_pitch = 89;
        }
        else if (m_pitch < -89)
        {
            m_pitch = -89;
        }
        float directionX, directionY, directionZ;
        directionX = std::cos(glm::radians(m_pitch)) * std::cos(glm::radians(m_yaw));
        directionY = std::sin(glm::radians(m_pitch));
        directionZ = -std::cos(glm::radians(m_pitch)) * std::sin(glm::radians(m_yaw));
        m_cameraFront = glm::normalize(glm::vec3(directionX, directionY, directionZ));
    }
    // 视口缩放
    void zoom(double yoffset)
    {
        m_fov += m_sensitivity * yoffset;
        if (m_fov > 45)
        {
            m_fov = 45;
        }
        if (m_fov < 1)
        {
            m_fov = 1;
        }
        return;
    }
};
