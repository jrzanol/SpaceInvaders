// by jrzanol
//

#pragma once

#include "CUtil.h"
#include "CModel.h"

class CGame : CEvent
{
public:
	CGame();
	~CGame();

	void Initialize();

	void ProcessInput(GLFWwindow*);
	void ProcessMouseButtonEvent(GLFWwindow*, int, int, int);
	void ProcessMiliSecTimer();
	void ProcessSecTimer();

	static bool m_GameOver;

	static glm::vec3* CheckBulletInMyWay(const glm::vec3*);
	static CModel* CheckMovement(glm::vec3, CModel* = NULL);
	static CModel* CheckBulletAttack(glm::vec3, CModel* = NULL, bool = false);

private:
	int m_Points;
};

