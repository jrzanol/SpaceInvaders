// by jrzanol
//

#pragma once

#include "CMesh.h"

const int MAX_OBJECT = 128;

class CModel
{
public:
	CModel();

	void Draw(GLuint, const glm::mat4&) const;

	glm::mat4& GetModelPos() const;
	glm::vec3* GetPosition() { return &m_Position; }

	std::string m_ObjName;
	std::string m_ObjDir;
	std::vector<CMesh*> m_Meshes;
	
	glm::vec3 m_Scale;
	float m_Angle;

	float m_TextCoord;
	int m_SelectedTexture;

	glm::vec3 m_InitPosition;
	float m_SpawnTime;
	bool m_StopMovement;
	int m_ModelType;

	static CModel* GetModel(int);
	static CModel* LoadModel(std::string, int&);
	static void DeleteModel(CModel*);

private:
	bool m_Atived;
	glm::vec3 m_Position;

	void ProcessModelNode(aiNode*, const aiScene*);
	CMesh* ProcessModelMesh(aiMesh*, const aiScene*);

	static CModel g_List[MAX_OBJECT];
};

