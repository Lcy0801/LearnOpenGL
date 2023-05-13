
// OpenGL���������
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

class Camera
{
public:
    // ���λ��
    glm::vec3 m_cameraPos;
    // ���z��
    glm::vec3 m_cameraFront;
    // ���up��
    glm::vec3 m_cameraUp;
    // �����̬ŷ����
    float m_yaw;
    float m_pitch;
    // ����ƶ��ٶ�
    float m_cameraSpeed;
    float m_sensitivity;
    // ����ӳ���
    float m_fov;
    Camera(glm::vec3 cameraPos, float yaw_, float pitch_, float speed, float sensitivity, float fov)
    {
        m_cameraPos = cameraPos;
        m_yaw = yaw_;
        m_pitch = pitch_;
        // ����ŷ���Ǽ�������۲췽������
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
    // ����ƶ�
    void move(GLFWwindow *window, float deltaTime)
    {
        // ǰ��
        if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        {
            m_cameraPos = m_cameraPos + m_cameraFront * m_cameraSpeed * deltaTime;
        }
        // ����
        if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        {
            m_cameraPos = m_cameraPos - m_cameraFront * m_cameraSpeed * deltaTime;
        }
        // ����
        if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        {
            m_cameraPos = m_cameraPos + normalize(cross(m_cameraUp, m_cameraFront)) * m_cameraSpeed * deltaTime;
        }
        // ����
        if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        {
            m_cameraPos = m_cameraPos + normalize(cross(m_cameraFront, m_cameraUp)) * m_cameraSpeed * deltaTime;
        }
    }
    // ��ͷת��
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
    // �ӿ�����
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
