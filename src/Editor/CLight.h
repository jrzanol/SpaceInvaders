// by jrzanol
//

#pragma once
#include "CEvent.h"

class CLight
{
public:
	CLight();

	void Draw(GLuint);
	void ProcessMiliSecTimer();
	void ProcessSecTimer();

private:
	int m_Id;

	glm::vec3 m_LightPos;
	glm::vec3 m_LightColor;
};

