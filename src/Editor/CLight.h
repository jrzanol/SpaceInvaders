// by jrzanol
//

#pragma once
#include "CEvent.h"

class CLight : CAnimation
{
public:
	CLight();

	void Reset(bool = true);
	void Draw(GLuint) const;

	void SetAnimation(bool = true);
	glm::vec3* GetPosition() { return &s_LightPos; }

	const char* ToString() const;

private:
	unsigned int m_LightId;

	glm::vec3 s_LightPos;
	glm::vec3 s_LightColor;

	static int s_LightCounter;
};

