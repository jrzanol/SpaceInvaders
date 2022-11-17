// by jrzanol
//

#pragma once
#include "CEvent.h"

class CLight
{
public:
	CLight();

	void Draw(GLuint) const;

private:
	glm::vec3 s_LightPos;
	glm::vec3 s_LightColor;
};

