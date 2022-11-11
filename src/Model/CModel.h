// by jrzanol
//

#pragma once

#include "CMesh.h"
#include "CAnimation.h"

const int MAX_OBJECT = 128;

class CModel : CAnimation
{
public:
	CModel();

	const char* ToString() const;
	void Draw(GLuint, const glm::mat4&) const;

	bool GetAnimation();
	void SetAnimation(bool = true);

	glm::mat4& GetModelPos() const;
	glm::vec3* GetPosition() { return &m_Position; }

	std::string m_ObjName;
	std::string m_ObjDir;
	std::vector<CMesh> m_Meshes;
	
	glm::vec3 m_Scale;
	float m_Angle;

	float m_TextCoord;
	int m_SelectedTexture;

	// Load File or Find Object.
	static CModel* LoadModel(std::string);

	static void Reset();
	static CModel* g_SelectedModel;

private:
	glm::vec3 m_Position;

	void ProcessModelNode(aiNode*, const aiScene*);
	CMesh ProcessModelMesh(aiMesh*, const aiScene*);

	static int g_ListCounter;
	static CModel g_List[MAX_OBJECT];
};

