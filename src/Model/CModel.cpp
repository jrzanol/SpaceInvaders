// by jrzanol
//

#include "stdafx.h"

#include "CUtil.h"
#include "CModel.h"

int CModel::g_ListCounter = 0;
CModel CModel::g_List[MAX_OBJECT];

CModel* CModel::g_SelectedModel = NULL;

CModel::CModel() : CAnimation()
{
    m_Position = glm::vec3(0.f, 0.f, 0.f);
    m_Scale = glm::vec3(1.f, 1.f, 1.f);
    m_Angle = 0.f;

    m_TextCoord = 0.f;
    m_SelectedTexture = 0;
}

void CModel::Reset()
{
    g_ListCounter = 0;

    for (int i = 0; i < MAX_OBJECT; ++i)
        g_List[i] = CModel();

    g_SelectedModel = NULL;
}

const char* CModel::ToString() const
{
    static char str[128];

    sprintf(str, "Obj[%s] %.2f,%.2f,%.2f %.2f,%.2f,%.2f %.2f %d", m_ObjName.c_str(), m_Position.x, m_Position.y, m_Position.z, m_Scale.x, m_Scale.y, m_Scale.z, m_Angle, m_SelectedTexture);
    return str;
}

void CModel::Draw(GLuint programId, const glm::mat4& vp) const
{
    if (g_SelectedModel == this)
        glUniform1i(glGetUniformLocation(programId, "u_wireframeColor"), 1);
    else
        glUniform1i(glGetUniformLocation(programId, "u_wireframeColor"), 0);

    glUniform1f(glGetUniformLocation(programId, "u_textcoord"), m_TextCoord);

    glm::mat4 model = GetModelPos();
    glUniformMatrix4fv(glGetUniformLocation(programId, "u_model"), 1, GL_FALSE, glm::value_ptr(model));

    for (const auto& it : m_Meshes)
        it.Draw(programId, m_SelectedTexture);
}

bool CModel::GetAnimation()
{
    return m_AniAtived;
}

void CModel::SetAnimation(bool v)
{
    m_AniAtived = v;

    if (m_AniAtived)
        FixAnimation(true);
}

glm::mat4& CModel::GetModelPos() const
{
    static glm::mat4 model;

    model = glm::mat4(1.f);
    model = glm::translate(model, m_Position);
    model = glm::scale(model, m_Scale);
    model = glm::rotate(model, glm::radians(m_Angle), glm::vec3(1.0f, 0.0f, 0.0f));

    return model;
}

CModel* CModel::LoadModel(std::string file)
{
    if (g_ListCounter >= MAX_OBJECT)
        return NULL;

    CModel* obj = &g_List[g_ListCounter];
    obj->m_ObjName = file;
    obj->m_ObjDir = file.substr(0, file.find('/'));

    Assimp::Importer importer;

    const aiScene* scene = importer.ReadFile(file, aiProcess_Triangulate | aiProcess_GenSmoothNormals | aiProcess_FlipUVs | aiProcess_CalcTangentSpace);
    if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
    {
        std::cout << "ERROR::ASSIMP:: " << importer.GetErrorString() << std::endl;
        return NULL;
    }

    // process ASSIMP's root node recursively
    obj->ProcessModelNode(scene->mRootNode, scene);
    obj->ReadAnimation(obj->m_ObjDir.c_str());

    if (g_SelectedModel == NULL)
        g_SelectedModel = obj;

    g_ListCounter++;
    return obj;
}

// processes a node in a recursive fashion. Processes each individual mesh located at the node and repeats this process on its children nodes (if any).
void CModel::ProcessModelNode(aiNode* node, const aiScene* scene)
{
    // process each mesh located at the current node
    for (unsigned int i = 0; i < node->mNumMeshes; i++)
    {
        // the node object only contains indices to index the actual objects in the scene. 
        // the scene contains all the data, node is just to keep stuff organized (like relations between nodes).
        aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
        m_Meshes.push_back(ProcessModelMesh(mesh, scene));
    }
    // after we've processed all of the meshes (if any) we then recursively process each of the children nodes
    for (unsigned int i = 0; i < node->mNumChildren; i++)
    {
        ProcessModelNode(node->mChildren[i], scene);
    }
}

CMesh CModel::ProcessModelMesh(aiMesh* mesh, const aiScene* scene)
{
    // data to fill
    std::vector<Vertex> vertices;
    std::vector<unsigned int> indices;
    std::vector<Texture> textures;

    // walk through each of the mesh's vertices
    for (unsigned int i = 0; i < mesh->mNumVertices; i++)
    {
        Vertex vertex;
        glm::vec3 vector; // we declare a placeholder vector since assimp uses its own vector class that doesn't directly convert to glm's vec3 class so we transfer the data to this placeholder glm::vec3 first.
        // positions
        vector.x = mesh->mVertices[i].x;
        vector.y = mesh->mVertices[i].y;
        vector.z = mesh->mVertices[i].z;
        vertex.Position = vector;
        // normals
        if (mesh->HasNormals())
        {
            vector.x = mesh->mNormals[i].x;
            vector.y = mesh->mNormals[i].y;
            vector.z = mesh->mNormals[i].z;
            vertex.Normal = vector;
        }
        // texture coordinates
        if (mesh->mTextureCoords[0]) // does the mesh contain texture coordinates?
        {
            glm::vec2 vec;
            // a vertex can contain up to 8 different texture coordinates. We thus make the assumption that we won't 
            // use models where a vertex can have multiple texture coordinates so we always take the first set (0).
            vec.x = mesh->mTextureCoords[0][i].x;
            vec.y = mesh->mTextureCoords[0][i].y;
            vertex.TexCoords = vec;
            // tangent
            vector.x = mesh->mTangents[i].x;
            vector.y = mesh->mTangents[i].y;
            vector.z = mesh->mTangents[i].z;
            vertex.Tangent = vector;
            // bitangent
            vector.x = mesh->mBitangents[i].x;
            vector.y = mesh->mBitangents[i].y;
            vector.z = mesh->mBitangents[i].z;
            vertex.Bitangent = vector;
        }
        else
            vertex.TexCoords = glm::vec2(0.0f, 0.0f);

        vertices.push_back(vertex);
    }
    // now wak through each of the mesh's faces (a face is a mesh its triangle) and retrieve the corresponding vertex indices.
    for (unsigned int i = 0; i < mesh->mNumFaces; i++)
    {
        aiFace face = mesh->mFaces[i];
        // retrieve all indices of the face and store them in the indices vector
        for (unsigned int j = 0; j < face.mNumIndices; j++)
            indices.push_back(face.mIndices[j]);
    }
    // process materials
    aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];
    // we assume a convention for sampler names in the shaders. Each diffuse texture should be named
    // as 'texture_diffuseN' where N is a sequential number ranging from 1 to MAX_SAMPLER_NUMBER. 
    // Same applies to other texture as the following list summarizes:
    // diffuse: texture_diffuseN
    // specular: texture_specularN
    // normal: texture_normalN

    // 1. diffuse maps
    std::vector<Texture> diffuseMaps = CTexture::LoadMaterialTextures(material, aiTextureType_DIFFUSE, "texture_diffuse", m_ObjDir.c_str());
    textures.insert(textures.end(), diffuseMaps.begin(), diffuseMaps.end());
    // 2. specular maps
    std::vector<Texture> specularMaps = CTexture::LoadMaterialTextures(material, aiTextureType_SPECULAR, "texture_specular", m_ObjDir.c_str());
    textures.insert(textures.end(), specularMaps.begin(), specularMaps.end());
    // 3. normal maps
    std::vector<Texture> normalMaps = CTexture::LoadMaterialTextures(material, aiTextureType_HEIGHT, "texture_normal", m_ObjDir.c_str());
    textures.insert(textures.end(), normalMaps.begin(), normalMaps.end());
    // 4. height maps
    std::vector<Texture> heightMaps = CTexture::LoadMaterialTextures(material, aiTextureType_AMBIENT, "texture_height", m_ObjDir.c_str());
    textures.insert(textures.end(), heightMaps.begin(), heightMaps.end());

    // return a mesh object created from the extracted mesh data
    return CMesh(vertices, indices, textures);
}

