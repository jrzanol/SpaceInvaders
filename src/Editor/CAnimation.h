// by jrzanol
//

#pragma once
#include "CEvent.h"

class CAnimation : CEvent
{
public:
	CAnimation();

	void ProcessMiliSecTimer();

protected:
	bool m_AniAtived;

	void ReadAnimation(const char*, const char* = "");
	void FixAnimation(bool = false);

	virtual glm::vec3* GetPosition() = 0;

private:
	unsigned int m_CurAni;
	bool m_Initialized;

	float m_InitAni;
	float m_LastAni;
	glm::vec3 m_InitPos;
	glm::vec3 m_StepAni;

	std::vector<std::tuple<glm::vec3, float>> m_Animation;
};

