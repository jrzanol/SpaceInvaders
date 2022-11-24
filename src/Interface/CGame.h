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

	static bool CheckMovement(glm::vec3, CModel* = NULL);
};

