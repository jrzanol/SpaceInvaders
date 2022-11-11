// by jrzanol
//

#include "stdafx.h"

#include "CUtil.h"
#include "CWindow.h"
#include "CAnimation.h"

CAnimation::CAnimation() : CEvent()
{
	m_AniAtived = false;
	m_Initialized = false;
	m_CurAni = 0;

	m_InitAni = 0.f;
	m_LastAni = 0.f;

	m_InitPos = glm::vec3();
	m_StepAni = glm::vec3();
}

void CAnimation::ProcessMiliSecTimer()
{
	if (!m_AniAtived || m_Animation.size() == 0)
		return;

	glm::vec3* pos = GetPosition();
	const float timer = g_LastTime;

	if (!m_Initialized)
		FixAnimation();

	const float diff = (timer - m_InitAni);
	if (diff > 0.f)
	{
		if (diff >= std::get<1>(m_Animation[m_CurAni]))
		{
			if (++m_CurAni >= m_Animation.size())
				m_CurAni = 0;

			FixAnimation();
		}
		else
		{
			*pos = m_InitPos + (m_StepAni * diff);
			m_LastAni = diff;
		}
	}
}

void CAnimation::ReadAnimation(const char* dir, const char* prefix)
{
	m_Initialized = false;
	m_Animation.clear();

	char file[32];
	sprintf(file, "%s/%sAni.txt", dir, prefix);

	FILE* in = fopen(file, "rt");
	if (in)
	{
		char line[128];

		while (fgets(line, sizeof(line), in))
		{
			if (line[0] == '\n' || line[0] == '#')
				continue;

			float x, y, z, timer;

			if (sscanf(line, "%f,%f,%f %f", &x, &y, &z, &timer) == 4)
				m_Animation.push_back(std::make_tuple(glm::vec3(x, y, z), timer));
		}

		fclose(in);
	}
}

void CAnimation::FixAnimation(bool fixTimer)
{
	glm::vec3* pos = GetPosition();
	const float timer = g_LastTime;

	if (!m_Initialized || !fixTimer)
	{
		m_Initialized = true;

		m_InitPos = *pos;
		m_InitAni = timer;
		m_StepAni = (std::get<0>(m_Animation[m_CurAni]) / std::get<1>(m_Animation[m_CurAni]));
	}
	else if (fixTimer)
		m_InitAni = (timer - m_LastAni);
}

