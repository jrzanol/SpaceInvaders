// by jrzanol
//

#pragma once

#include "CTexture.h"

struct Vertex
{
    glm::vec3 Position;
    glm::vec3 Normal;
    glm::vec2 TexCoords;
    glm::vec3 Tangent;
    glm::vec3 Bitangent;

	void Clear()
	{
		Position.x = 0.f;
		Position.y = 0.f;
		Position.z = 0.f;
		Normal.x = 0.f;
		Normal.y = 0.f;
		Normal.z = 0.f;
		TexCoords.x = 0.f;
		TexCoords.y = 0.f;
		Tangent.x = 0.f;
		Tangent.y = 0.f;
		Tangent.z = 0.f;
		Bitangent.x = 0.f;
		Bitangent.y = 0.f;
		Bitangent.z = 0.f;
	}

	inline bool operator==(const Vertex& v)
	{
		return this == &v;
	}
};

const int MAX_MESH = 32;

class CMesh
{
public:
	CMesh();

	void Draw(GLuint) const;
	void AllocBuffer();

	std::string m_ObjName;
	std::vector<Vertex> m_Vertex;
	std::vector<Texture> m_Textures;
	std::vector<unsigned int> m_Indices;

	static std::vector<CMesh*> Get(const std::string);
	static CMesh* Load(const std::string, const std::vector<Vertex>&, const std::vector<unsigned int>&, const std::vector<Texture>&);

private:
	GLuint m_VBOId;
	GLuint m_EBOId;
	GLuint m_VAOId;

	void CalculateNormals();

	static CMesh g_Mesh[MAX_MESH];
	static unsigned int g_MeshCounter;
};

