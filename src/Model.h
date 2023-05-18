#ifndef MODEL_H
#define MODEL_H
#include <string>
#include <iostream>
#include <vector>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
// OpenGL���������
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <Shader.h>
// ģ�͵����
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
// ͼ����ؿ�
#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

// ��ͼ����
enum TextureType
{
    DIFFUSE = 1, // ��������ͼ
    SPECULAR = 2 // ���淴����ͼ
};

// ����ṹ��
struct Vertext
{
    glm::vec3 Position;
    glm::vec3 Normal;
    glm::vec2 Textcoord;
    Vertext(glm::vec3 position, glm::vec3 normal, glm::vec2 textcoord) : Position(position), Normal(normal), Textcoord(textcoord){};
};

// �������ṹ��
struct Texture
{
    unsigned int id;
    TextureType type;
    std::string path;
};

class Mesh
{
private:
    unsigned int m_vao, m_vbo, m_ibo;
    void setupMesh()
    {
        glGenVertexArrays(1, &m_vao);
        glBindVertexArray(m_vao);
        glGenBuffers(1, &m_vbo);
        glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
        glBufferData(GL_ARRAY_BUFFER, sizeof(Vertext) * m_vertices.size(), &m_vertices[0], GL_STATIC_DRAW);
        glGenBuffers(1, &m_ibo);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_ibo);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GL_INT) * m_indices.size(), &m_indices[0], GL_STATIC_DRAW);
        // ���Ӷ�������
        // ��������
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, false, 8 * sizeof(GL_FLOAT), (void *)offsetof(Vertext, Position));
        // ���㷨��
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 3, GL_FLOAT, false, 8 * sizeof(GL_FLOAT), (void *)offsetof(Vertext, Normal));
        // ������������
        glEnableVertexAttribArray(2);
        glVertexAttribPointer(2, 2, GL_FLOAT, false, 8 * sizeof(GL_FLOAT), (void *)offsetof(Vertext, Textcoord));
        glBindVertexArray(0);
    }

public:
    std::vector<Vertext> m_vertices;
    std::vector<unsigned int> m_indices;
    std::vector<Texture> m_textures;
    Mesh(const std::vector<Vertext> &vertices, const std::vector<unsigned int> &indices, const std::vector<Texture> &textures)
    {
        m_vertices = vertices;
        m_indices = indices;
        m_textures = textures;
        setupMesh();
    }
    void Draw(Shader shader)
    {
        unsigned int diffsueNr = 1;
        unsigned int specularNr = 1;
        shader.use();
        for (int i = 0; i < m_textures.size(); i++)
        {
            glActiveTexture(GL_TEXTURE0 + i);
            Texture texture = m_textures[i];
            glBindTexture(GL_TEXTURE_2D, texture.id);
            // �����������ɫ���ж�Ӧ�Ĳ�������������
            std::string samplerName;
            if (texture.type == DIFFUSE)
            {
                samplerName = "material.diffuseMap_" + std::to_string(diffsueNr);
                diffsueNr++;
            }
            else if (texture.type == SPECULAR)
            {
                samplerName = "material.specularMap_" + std::to_string(specularNr);
                specularNr++;
            }
            shader.setUniformInt(samplerName, i);
        }
        glActiveTexture(GL_TEXTURE0);
        // ��������
        glBindVertexArray(m_vao);
        glDrawElements(GL_TRIANGLES, m_indices.size(), GL_UNSIGNED_INT, 0);
        glBindVertexArray(0);
    }
};

class Model
{
private:
    void loadModel(std::string path)
    {
        Assimp::Importer import;
        const aiScene *scene = import.ReadFile(path, aiProcess_Triangulate | aiProcess_GenSmoothNormals | aiProcess_FlipUVs);
        if (!scene || (scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE) || !scene->mRootNode)
        {
            std::cout << "ģ�͵���ʧ��!" << std::endl;
            std::cout << import.GetErrorString() << std::endl;
            return;
        }
        m_directory = path.substr(0, path.find_last_of('/'));
        processNode(scene->mRootNode, scene);
    }
    void processNode(aiNode *node, const aiScene *scene)
    {
        for (int i = 0; i < node->mNumMeshes; i++)
        {
            int meshIndex = node->mMeshes[i];
            aiMesh *mesh = scene->mMeshes[meshIndex];
            m_meshes.push_back(processMesh(mesh, scene));
        }
        for (int i = 0; i < node->mNumChildren; i++)
        {
            processNode(node->mChildren[i], scene);
        }
    }
    Mesh processMesh(aiMesh *mesh, const aiScene *scene)
    {
        std::vector<Vertext> vertices;
        std::vector<unsigned int> indices;
        std::vector<Texture> textures;
        // �������ݴ���
        for (int i = 0; i < mesh->mNumVertices; i++)
        {
            glm::vec3 position = glm::vec3(mesh->mVertices[i].x, mesh->mVertices[i].y, mesh->mVertices[i].z);
            glm::vec3 normal = glm::vec3(mesh->mNormals[i].x, mesh->mNormals[i].y, mesh->mNormals[i].z);
            glm::vec2 textcoord;
            // std::cout << position.x << "\t" << position.y << "\t" << position.z << std::endl;
            if (mesh->mTextureCoords[0])
            {
                textcoord = glm::vec2(mesh->mTextureCoords[0][i].x,mesh->mTextureCoords[0][i].y);
            }
            else
            {
                textcoord = glm::vec2(0, 0);
            }
            Vertext vertex(position, normal, textcoord);
            vertices.push_back(vertex);
        }
        // �������ݴ���
        for (int i = 0; i < mesh->mNumFaces; i++)
        {
            aiFace face = mesh->mFaces[i];
            for (int j = 0; j < face.mNumIndices; j++)
            {
                indices.push_back(face.mIndices[j]);
            }
        }
        // �������ݴ���
        if (mesh->mMaterialIndex >= 0)
        {
            aiMaterial *material = scene->mMaterials[mesh->mMaterialIndex];
            std::vector<Texture> diffuseMaps = loadMaterialTextures(material, aiTextureType_DIFFUSE, DIFFUSE);
            textures.insert(textures.end(), diffuseMaps.begin(), diffuseMaps.end());
            std::vector<Texture> specularMaps = loadMaterialTextures(material, aiTextureType_SPECULAR, SPECULAR);
            textures.insert(textures.end(), specularMaps.begin(), specularMaps.end());
        }
        return Mesh(vertices, indices, textures);
    }
    std::vector<Texture> loadMaterialTextures(aiMaterial *mat, aiTextureType type, TextureType typeName)
    {
        auto loadTexture = [](const char *imageFile, std::string directory)
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
            std::string imagePath;
            imagePath = directory + '/' + imageFile;
            int width, height, nChannels;
            unsigned char *textureData = stbi_load(imagePath.c_str(), &width, &height, &nChannels, 0);
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

        std::vector<Texture> textures;

        for (int i = 0; i < mat->GetTextureCount(type); i++)
        {
            aiString str;
            mat->GetTexture(type, i, &str);
            bool skip = false;
            // ��ֹģ�������ظ����� ��������Ч��
            for (auto &textureLoaded : m_texturesLoaded)
            {
                if (strcmp(textureLoaded.path.c_str(), str.data) == 0)
                {
                    textures.push_back(textureLoaded);
                    skip = true;
                    break;
                }
            }
            if (!skip)
            {
                Texture texture;
                texture.id = loadTexture(str.C_Str(), m_directory);
                texture.type = typeName;
                texture.path = str.C_Str();
                textures.push_back(texture);
                m_texturesLoaded.push_back(texture);
            }
        }
        return textures;
    }

public:
    std::vector<Mesh> m_meshes;
    std::string m_directory;
    // ģ�������Ѿ����ع�������
    std::vector<Texture> m_texturesLoaded;
    Model(char *path)
    {
        loadModel(path);
    }
    void Draw(Shader shader)
    {
        // ������������
        for (auto mesh : m_meshes)
        {
            mesh.Draw(shader);
        }
    }
};
#endif