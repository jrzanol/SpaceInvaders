// by jrzanol
//

#include "stdafx.h"

#include "CUtil.h"
#include "CWindow.h"
#include "CLight.h"

int CLight::s_LightCounter = 0;

CLight::CLight() : CAnimation()
{
	Reset(false);
}

void CLight::Reset(bool resetCounter)
{
	if (resetCounter)
		s_LightCounter = 0;

	if (s_LightCounter == 0)
	{
		s_LightPos = glm::vec3(5.02f, -0.18f, 0.24f);
		s_LightColor = glm::vec3(1.f, 1.f, 1.f);
	}
	else if (s_LightCounter == 1)
	{
		s_LightPos = glm::vec3(-3.17f, 0.63f, 3.1f);
		s_LightColor = glm::vec3(1.f, 0.f, 0.f);
	}
	else
	{
		s_LightPos = glm::vec3(0.13f, 8.1f, -0.26f);
		s_LightColor = glm::vec3(0.f, 1.f, 0.f);
	}

	m_LightId = s_LightCounter;
	s_LightCounter++;

	char prefix[16];
	sprintf(prefix, "Light%d", s_LightCounter);

	ReadAnimation(".", prefix);
}

void CLight::SetAnimation(bool v)
{
	m_AniAtived = v;

	if (m_AniAtived)
		FixAnimation(true);
}

void CLight::Draw(GLuint programId) const
{
	char lightPosStr[16], lightColorStr[16];
	sprintf(lightPosStr, "lightPos%s", m_LightId == 0 ? "" : std::to_string(m_LightId + 1).c_str());
	sprintf(lightColorStr, "lightColor%s", m_LightId == 0 ? "" : std::to_string(m_LightId + 1).c_str());

	glUniform3fv(glGetUniformLocation(programId, lightPosStr), 1, glm::value_ptr(s_LightPos));
	glUniform3fv(glGetUniformLocation(programId, lightColorStr), 1, glm::value_ptr(s_LightColor));
}

const char* CLight::ToString() const
{
	static char str[128];

	sprintf(str, "Light#%d %.2f,%.2f,%.2f", m_LightId + 1, s_LightPos.x, s_LightPos.y, s_LightPos.z);
	return str;
}

