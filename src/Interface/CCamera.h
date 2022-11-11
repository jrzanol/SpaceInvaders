// by jrzanol
//

#pragma once

#include "CEvent.h"
#include "CAnimation.h"

class CCamera : CAnimation
{
public:
	CCamera();
	~CCamera();

	void Reset(bool = true);

	void ProcessSecTimer();
	void ProcessInput(GLFWwindow*);

	void ProcessMouseButtonEvent(GLFWwindow*, int, int, int);
	void ProcessMouseDragEvent(GLFWwindow*, float, float);
	void ProcessMouseScroll(GLFWwindow*, double, double);

	static int m_CameraId;

	// Returns the view matrix calculated using Euler Angles and the LookAt Matrix.
	glm::mat4 GetViewMatrix();
	glm::vec3* GetPosition() { return &m_Position; }

	const char* ToString() const;
	void SetAnimation(bool = true);

	float m_Zoom;

private:
	int m_CamId;

	// Camera Attributes.
	glm::vec3 m_Position;
	glm::vec3 m_Front;
	glm::vec3 m_Up;
	glm::vec3 m_Right;
	glm::vec3 m_WorldUp;

	// Last x,y mouse position.
	float m_LastX;
	float m_LastY;

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

	// Processes input received from any keyboard-like input system. Accepts input parameter in the form of camera defined ENUM (to abstract it from windowing systems)
	void ProcessKeyboard(Camera_Movement, float);

	// Calculates the front vector from the Camera's (updated) Euler Angles.
	void UpdateCameraVectors();

	// Default Camera values.
	const float YAW = -90.0f;
	const float PITCH = 0.0f;
	const float SPEED = 2.5f;
	const float SENSITIVITY = 0.1f;
	const float ZOOM = 45.0f;

	static int si_CameraCounter;
};

