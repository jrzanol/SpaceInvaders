// by jrzanol
//

#include "stdafx.h"

#include "CUtil.h"
#include "CWindow.h"
#include "CLight.h"

CLight::CLight()
{
	m_LightPos = glm::vec3(0.f, 0.f, 0.f);
	m_LightColor = glm::vec3(1.f, 1.f, 1.f);

	static int g_LightIdCounter = 0;
	m_Id = g_LightIdCounter++;
}

void CLight::Draw(GLuint programId)
{
	if (m_Id == 0)
	{
		m_LightPos = *CModel::GetModel(0)->GetPosition();
		m_LightPos.y = 1.5f;
	}

	char lightPosStr[16], lightColorStr[16];
	sprintf(lightPosStr, "lightPos%s", m_Id == 0 ? "" : std::to_string(m_Id + 1).c_str());
	sprintf(lightColorStr, "lightColor%s", m_Id == 0 ? "" : std::to_string(m_Id + 1).c_str());

	glUniform3fv(glGetUniformLocation(programId, lightPosStr), 1, glm::value_ptr(m_LightPos));
	glUniform3fv(glGetUniformLocation(programId, lightColorStr), 1, glm::value_ptr(m_LightColor));
}

void CLight::ProcessMiliSecTimer()
{
}

void CLight::ProcessSecTimer()
{
}

