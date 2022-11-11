// by jrzanol
//

#pragma once

const int MAX_TEXTURE = 16;

struct Texture
{
    unsigned int m_Id;
    std::string m_Type;
    std::string m_Path;
};

class CTexture
{
public:
    static std::vector<Texture> LoadMaterialTextures(aiMaterial*, aiTextureType, std::string, std::string);

private:
    static bool g_Initialized;
    static int g_ListCounter;
    static Texture g_List[MAX_TEXTURE];

    static unsigned int TextureFromFile(const char*, const std::string&, bool = true);
};

