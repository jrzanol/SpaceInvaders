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

	// Defines several possible options for camera movement. Used as abstraction to stay away from window-system specific input methods.
	enum class Camera_Movement {
		FORWARD,
		BACKWARD,
		LEFT,
		RIGHT
	};

	// Calculates the front vector from the Camera's (updated) Euler Angles.
	void UpdateCameraVectors();

	// Default Camera values.
	const float YAW = -90.0f;
	const float PITCH = 0.0f;
	const float SPEED = 2.5f;
	const float SENSITIVITY = 0.1f;
	const float ZOOM = 45.0f;
};

