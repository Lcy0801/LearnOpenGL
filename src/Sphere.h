#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <vector>
#include <Shader.h>
#define PI 3.1415926
class Sphere
{
private:
    unsigned int m_vao;
    unsigned int m_vbo;
    unsigned int m_ibo;
    std::vector<unsigned int> m_sphereIndices;
    std::vector<float> m_sphereVertices;
    init()
    {
        // 球面顶点坐标
        float dxAngle = 2 * PI / m_X_SEGMENTS;
        float dyAngle = PI / m_Y_SEGMENTS;
        for (int i = 0; i <= m_Y_SEGMENTS; i++)
        {
            float y = cos(i * dyAngle);
            for (int j = 0; j <= m_X_SEGMENTS; j++)
            {
                float x = sin(i * dyAngle) * cos(j * dxAngle);
                float z = sin(i * dyAngle) * sin(j * dxAngle);
                m_sphereVertices.push_back(x);
                m_sphereVertices.push_back(y);
                m_sphereVertices.push_back(z);
            }
        }
        // 球面三角对应的顶点索引
        for (int i = 0; i < m_Y_SEGMENTS; i++)
        {
            for (int j = 0; j < m_X_SEGMENTS; j++)
            {
                int index1 = i * (m_X_SEGMENTS + 1) + j;
                int index2 = i * (m_X_SEGMENTS + 1) + j + 1;
                int index3 = (i + 1) * (m_X_SEGMENTS + 1) + j;
                int index4 = (i + 1) * (m_X_SEGMENTS + 1) + j + 1;
                m_sphereIndices.push_back(index1);
                m_sphereIndices.push_back(index2);
                m_sphereIndices.push_back(index4);
                m_sphereIndices.push_back(index1);
                m_sphereIndices.push_back(index3);
                m_sphereIndices.push_back(index4);
            }
        }
        glGenVertexArrays(1, &m_vao);
        glBindVertexArray(m_vao);
        glGenBuffers(1, &m_vbo);
        glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
        glBufferData(GL_ARRAY_BUFFER, sizeof(GL_FLOAT) * m_sphereVertices.size(), &m_sphereVertices[0], GL_STATIC_DRAW);
        glVertexAttribPointer(0, 3, GL_FLOAT, false, 3 * sizeof(GL_FLOAT), 0);
        glEnableVertexAttribArray(0);
        glGenBuffers(1, &m_ibo);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_ibo);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GL_INT) * m_sphereIndices.size(), &m_sphereIndices[0], GL_STATIC_DRAW);
        glBindVertexArray(0);
    }

public:
    int m_X_SEGMENTS;
    int m_Y_SEGMENTS;
    double m_RAIDUS;
    Sphere(int X_SEGMENTS, int Y_SEGMENTS, double RAIDUS)
    {
        m_X_SEGMENTS = X_SEGMENTS;
        m_Y_SEGMENTS = Y_SEGMENTS;
        m_RAIDUS = RAIDUS;
        init();
    }
    void Draw(Shader shader)
    {
        glBindVertexArray(m_vao);
        glDrawElements(GL_TRIANGLES, m_sphereIndices.size(), GL_UNSIGNED_INT, 0);
    }
};
