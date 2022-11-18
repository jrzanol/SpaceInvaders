// by jrzanol
//

#pragma once

#include "CEvent.h"

class CCamera
{
public:
	CCamera();
	~CCamera();

	void ProcessSecTimer();
	void ProcessInput(GLFWwindow*);

	// Returns the view matrix calculated using Euler Angles and the LookAt Matrix.
	glm::mat4 GetViewMatrix();

	// Camera Zoom for calcule Projetion matrix.
	float m_Zoom;

private:
	// Camera Attributes.
	glm::vec3 m_Position;
	glm::vec3 m_Front;
	glm::vec3 m_Up;
	glm::vec3 m_Right;
	glm::vec3 m_WorldUp;

	// Euler Angles.
	float m_Yaw;
	float m_Pitch;

	// Calculates the front vector from the Camera's (updated) Euler Angles.
	void UpdateCameraVectors();
};

