// by jrzanol
//

#pragma once

#include "CUtil.h"
#include "CCamera.h"
#include "CModel.h"
#include "CLight.h"
#include "CGame.h"

class CWindow
{
public:
	CWindow();
	~CWindow();

	bool Initialize();
	void Cleanup();
	bool Render();

	static const glm::mat4& GetVP();

	static CModel* GetModel(unsigned int);
	static unsigned int GetModelCount();

	static const unsigned int MAX_MODEL = 64;
	static CModel* CreateModel(int, const char*);

private:
	CGame m_Game;
	CLight m_Light;
	CCamera m_Camera;
	
	GLuint m_ProgramId;

	GLuint CompileShader(const char*, GLenum);
	GLuint LinkProgram(GLuint, GLuint);

	static glm::mat4 m_VP;

	static unsigned int m_ModelCounter;
	static CModel* m_DrawModel[MAX_MODEL];
};

