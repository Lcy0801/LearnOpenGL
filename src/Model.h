#ifndef MODEL_H
#define MODEL_H
#include <string>
#include <iostream>
#include <vector>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
// OpenGL矩阵运算库
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <Shader.h>
// 模型导入库
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

// 贴图类型
enum TextureType
{
    DIFFUSE = 1, // 漫反射贴图
    SPECULAR = 2 // 镜面反射贴图
};

// 顶点结构体
struct Vertext
{
    glm::vec3 Position;
    glm::vec3 Normal;
    glm::vec2 Textcoord;
    Vertext(glm::vec3 position, glm::vec3 normal, glm::vec2 textcoord) : Position(position), Normal(normal), Textcoord(textcoord){};
};

// 纹理对象结构体
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
        glBufferData(GL_ARRAY_BUFFER, sizeof(Vertext) * m_vertices.size(), &m_textures[0], GL_STATIC_DRAW);
        glGenBuffers(1, &m_ibo);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_ibo);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GL_INT) * m_indices.size(), &m_indices[0], GL_STATIC_DRAW);
        // 链接顶点属性
        // 顶点坐标
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, false, 8 * sizeof(GL_FLOAT), (void *)offsetof(Vertext, Position));
        // 顶点法线
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 3, GL_FLOAT, false, 8 * sizeof(GL_FLOAT), (void *)offsetof(Vertext, Normal));
        // 顶点纹理坐标
        glEnableVertexAttribArray(2);
        glVertexAttribPointer(2, 2, GL_FLOAT, false, 8 * sizeof(GL_FLOAT), (void *)offsetof(Vertext, Textcoord));
        glBindVertexArray(0);
    }

public:
    std::vector<Vertext> m_vertices;
    std::vector<int> m_indices;
    std::vector<Texture> m_textures;
    Mesh(const std::vector<Vertext> &vertices, const std::vector<int> &indices, const std::vector<Texture> &textures)
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
        for (int i = 0; i < m_textures.size(); i++)
        {
            glActiveTexture(GL_TEXTURE0 + i);
            Texture texture = m_textures[i];
            glBindTexture(GL_TEXTURE_2D, texture.id);
            // 纹理对象在着色器中对应的采样器变量名称
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
            shader.setUniformInt(samplerName, GL_TEXTURE0 + i);
        }
        glActiveTexture(GL_TEXTURE0);
        // 绘制网格
        glBindVertexArray(m_vao);
        glDrawElements(GL_TRIANGLES, m_indices.size(), GL_INT, 0);
        glBindVertexArray(0);
    }
};

class Model
{
private:
    std::vector<Mesh> m_meshes;
    std::string m_directory;
    // 模型所有已经加载过的纹理
    std::vector<Texture> m_texturesLoaded;
    void loadModel(std::string path)
    {
        Assimp::Importer import;
        const aiScene *scene = import.ReadFile(path, aiProcess_Triangulate | aiProcess_GenNormals | aiProcess_FlipUVs);
        if (!scene || scene->mFlags | AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
        {
            std::cout << "模型导入失败!" << std::endl;
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
        std::vector<int> indices;
        std::vector<Texture> textures;
        // 顶点数据处理
        for (int i = 0; i < mesh->mNumVertices; i++)
        {
            glm::vec3 position = glm::vec3(mesh->mVertices[i].x, mesh->mVertices[i].y, mesh->mVertices[i].z);
            glm::vec3 normal = glm::vec3(mesh->mNormals[i].x, mesh->mNormals[i].y, mesh->mNormals[i].z);
            glm::vec2 textcoord;
            if (mesh->mTextureCoords[0])
            {
                textcoord = glm::vec2(mesh->mTextureCoords[0][i].x);
            }
            else
            {
                textcoord = glm::vec2(0, 0);
            }
            Vertext vertex(position, normal, textcoord);
            vertices.push_back(vertex);
        }
        // 索引数据处理
        for (int i = 0; i < mesh->mNumFaces; i++)
        {
            aiFace face = mesh->mFaces[i];
            for (int j = 0; j < face.mNumIndices; j++)
            {
                indices.push_back(face.mIndices[j]);
            }
        }
        // 纹理数据处理
        if (mesh->mMaterialIndex >= 0)
        {
            aiMaterial *material = scene->mMaterials[mesh->mMaterialIndex];
            std::vector<Texture> diffuseMaps = loadMaterialTextures(material, aiTextureType_DIFFUSE, DIFFUSE);
            textures.insert(textures.end(), diffuseMaps.begin(), diffuseMaps.end());
            std::vector<Texture> specularMaps = loadMaterialTextures(material, aiTextureType_SPECULAR, SPECULAR);
            textures.insert(textures.end(), specularMaps.begin(), specularMaps.end());
        }
    }
    std::vector<Texture> loadMaterialTextures(aiMaterial *mat, aiTextureType type, TextureType typeName)
    {
        auto loadTexture = [](const char *imageFile, std::string directory)
        {
            // 纹理
            unsigned int textureId;
            glGenTextures(1, &textureId);
            // 设置纹理环绕方式
            glTextureParameteri(textureId, GL_TEXTURE_WRAP_S, GL_REPEAT);
            glTextureParameteri(textureId, GL_TEXTURE_WRAP_T, GL_REPEAT);
            // 设置纹理过滤方式
            glTextureParameteri(textureId, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
            glTextureParameteri(textureId, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
            // 纹理贴图
            std::string imagePath;
            imagePath = directory + '/' + imageFile;
            int width, height, nChannels;
            unsigned char *textureData = stbi_load(imagePath, &width, &height, &nChannels, 0);
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
                // 绑定纹理
                glBindTexture(GL_TEXTURE_2D, textureId);
                glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, textureData);
                glGenerateMipmap(GL_TEXTURE_2D);
                // 释放贴图内存
                stbi_image_free(textureData);
            }
            else
            {
                std::cout << imageFile << std::endl;
                std::cout << "该纹理贴图载入失败!" << std::endl;
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
            // 防止模型纹理重复加载 提升加载效率
            for (auto &textureLoaded : m_texturesLoaded)
            {
                if (strcmp(textureLoaded.path.c_str(), str.data()) == 0)
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
    Model(char *path)
    {
        loadModel(path);
    }
    void Draw(Shader shader)
    {
        // 绘制所有网格
        for (auto mesh : m_meshes)
        {
            mesh.Draw(shader);
        }
    }
};
#endif