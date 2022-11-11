// by jrzanol
//

#pragma once

#include "CUtil.h"
#include "CCamera.h"
#include "CModel.h"
#include "CLight.h"

class CWindow
{
public:
	CWindow();
	~CWindow();

	bool Initialize();
	void Cleanup();
	bool Render();

	static const glm::mat4& GetVP();
	static const std::list<CModel*>& GetModels();

private:
	CLight m_Light[3];
	CCamera m_Camera[3];
	
	GLuint m_ProgramId;
	GLuint m_PickingProgramId;

	GLuint CompileShader(const char*, GLenum);
	GLuint LinkProgram(GLuint, GLuint);

	static glm::mat4 m_VP;
	static std::list<CModel*> m_DrawModel;

	static void CreateModel(int, const char*);
	static void SaveModel();
};

