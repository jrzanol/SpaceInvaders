// by jrzanol
//

#include "stdafx.h"
#include "CTexture.h"

#include <stb_image.h>

bool CTexture::g_Initialized = false;
int CTexture::g_ListCounter = 0;
Texture CTexture::g_List[MAX_TEXTURE];

std::vector<Texture> CTexture::LoadMaterialTextures(aiMaterial* mat, aiTextureType type, std::string typeName, std::string directory)
{
    if (!g_Initialized)
    {
        for (unsigned int i = 0; i < MAX_TEXTURE; i++)
            g_List[i].m_Id = -1;

        // tell stb_image.h to flip loaded texture's on the y-axis (before loading model).
        //stbi_set_flip_vertically_on_load(true);

        g_Initialized = true;
    }

    std::vector<Texture> textures;

    unsigned int maxTex = mat->GetTextureCount(type);
    if (maxTex > 0)
    {
        for (unsigned int i = 0; i < maxTex; i++)
        {
            aiString str;
            mat->GetTexture(type, i, &str);

            // check if texture was loaded before and if so, continue to next iteration: skip loading a new texture
            bool skip = false;

            for (int j = 0; j < g_ListCounter; j++)
            {
                if (std::strcmp(g_List[j].m_Path.data(), str.C_Str()) == 0)
                {
                    textures.push_back(g_List[j]);
                    skip = true; // a texture with the same filepath has already been loaded, continue to next one. (optimization)
                    break;
                }
            }

            if (!skip)
            {   // if texture hasn't been loaded already, load it
                for (int j = g_ListCounter; j < MAX_TEXTURE; j++)
                {
                    if (g_List[j].m_Id == -1)
                    {
                        g_List[j].m_Id = TextureFromFile(str.C_Str(), directory);
                        g_List[j].m_Type = typeName;
                        g_List[j].m_Path = str.C_Str();
                        g_ListCounter++;

                        textures.push_back(g_List[j]);
                        break;
                    }
                }
            }
        }
    }

    return textures;
}

unsigned int CTexture::TextureFromFile(const char* path, const std::string& directory, bool gamma)
{
    std::string filename = std::string(path);
    filename = directory + '/' + filename;

    unsigned int textureID;
    glGenTextures(1, &textureID);

    int width, height, nrComponents;
    unsigned char* data = stbi_load(filename.c_str(), &width, &height, &nrComponents, 0);
    if (data)
    {
        GLenum format = 0;

        if (nrComponents == 1)
            format = GL_RED;
        else if (nrComponents == 3)
            format = GL_RGB;
        else if (nrComponents == 4)
            format = GL_RGBA;

        glBindTexture(GL_TEXTURE_2D, textureID);
        glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        stbi_image_free(data);
    }
    else
    {
        std::cout << "Texture failed to load at path: " << path << std::endl;
        stbi_image_free(data);
    }

    return textureID;
}
