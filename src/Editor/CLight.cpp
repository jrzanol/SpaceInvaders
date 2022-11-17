// by jrzanol
//

#include "stdafx.h"

#include "CUtil.h"
#include "CWindow.h"
#include "CLight.h"

CLight::CLight()
{
	s_LightPos = glm::vec3(5.02f, -0.18f, 0.24f);
	s_LightColor = glm::vec3(1.f, 1.f, 1.f);
}

void CLight::Draw(GLuint programId) const
{
	int lightId = 0;

	char lightPosStr[16], lightColorStr[16];
	sprintf(lightPosStr, "lightPos%s", lightId == 0 ? "" : std::to_string(lightId + 1).c_str());
	sprintf(lightColorStr, "lightColor%s", lightId == 0 ? "" : std::to_string(lightId + 1).c_str());

	glUniform3fv(glGetUniformLocation(programId, lightPosStr), 1, glm::value_ptr(s_LightPos));
	glUniform3fv(glGetUniformLocation(programId, lightColorStr), 1, glm::value_ptr(s_LightColor));
}

