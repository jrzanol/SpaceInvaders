// by jrzanol
//

#include "stdafx.h"

#include "CUtil.h"
#include "CModel.h"
#include "CCamera.h"

CCamera::CCamera()
{
    m_Position = glm::vec3(0.0f, 30.f, 0.f);
    m_WorldUp = glm::vec3(0.0f, 1.0f, 0.0f);
    m_Yaw = -90.f;
    m_Pitch = -71.4f;
    m_Zoom = 45.f;

    UpdateCameraVectors();
}

CCamera::~CCamera()
{
}

void CCamera::ProcessSecTimer()
{
#ifdef _DEBUG
    static time_t s_LastShowCamPosition = 0;
    time_t rawnow = time(0);

    if (s_LastShowCamPosition < rawnow)
    {
        printf("Pos %.2f,%.2f,%.2f\n", m_Position.x, m_Position.y, m_Position.z);
        s_LastShowCamPosition = rawnow + 10;
    }

    //printf("Pos %.2f,%.2f,%.2f WorldUp %.2f,%.2f,%.2f Yan %.2f Pitch %.2f Zoom %.2f\n", m_Position.x, m_Position.y, m_Position.z, m_WorldUp.x, m_WorldUp.y, m_WorldUp.z, m_Yaw, m_Pitch, m_Zoom);
#endif
}

void CCamera::ProcessInput(GLFWwindow* window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
}

glm::mat4 CCamera::GetViewMatrix()
{
    return glm::lookAt(m_Position, m_Position + m_Front, m_Up);
}

void CCamera::UpdateCameraVectors()
{
    // Calculate the new Front vector.
    glm::vec3 front;
    front.x = cos(glm::radians(m_Yaw)) * cos(glm::radians(m_Pitch));
    front.y = sin(glm::radians(m_Pitch));
    front.z = sin(glm::radians(m_Yaw)) * cos(glm::radians(m_Pitch));

    m_Front = glm::normalize(front);
    float angle = 180.f/atan2f(m_Front.y, m_Front.x);

    // also re-calculate the Right and Up vector
    m_Right = glm::normalize(glm::cross(m_Front, m_WorldUp));  // normalize the vectors, because their length gets closer to 0 the more you look up or down which results in slower movement.
    m_Up = glm::normalize(glm::cross(m_Right, m_Front));
}

