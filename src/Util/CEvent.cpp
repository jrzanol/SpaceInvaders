// by jrzanol
//

#include "stdafx.h"

#include "CUtil.h"
#include "CEvent.h"
#include "CModel.h"

std::vector<CEvent*>* g_EventList;

bool g_FirstMouse = true;
float g_LastX = 0.f;
float g_LastY = 0.f;

CEvent::CEvent()
{
	if (g_EventList == NULL)
		g_EventList = new std::vector<CEvent*>();

	m_Priority = 0;
	g_EventList->push_back(this);
}

void MouseEvent_Button_Callback(GLFWwindow* window, int button, int action, int mods)
{
	const auto& io = ImGui::GetIO();

	if (io.WantCaptureMouse || io.WantCaptureKeyboard)
		return;

	if (button == GLFW_MOUSE_BUTTON_LEFT)
	{
		if (action == GLFW_PRESS)
			g_FirstMouse = false;
		else if (action == GLFW_RELEASE)
			g_FirstMouse = true;
	}

	for (const auto& it : *g_EventList)
		it->ProcessMouseButtonEvent(window, button, action, mods);
}

void MouseEvent_Callback(GLFWwindow* window, double xposIn, double yposIn)
{
	const auto& io = ImGui::GetIO();

	if (io.WantCaptureMouse || io.WantCaptureKeyboard)
		return;

	for (const auto& it : *g_EventList)
		it->ProcessMouseEvent(window, xposIn, yposIn);

	float xpos = static_cast<float>(xposIn);
	float ypos = static_cast<float>(yposIn);

	if (!g_FirstMouse)
	{
		float xoffset = xpos - g_LastX;
		float yoffset = g_LastY - ypos; // reversed since y-coordinates go from bottom to top

		g_LastX = xpos;
		g_LastY = ypos;

		for (const auto& it : *g_EventList)
			it->ProcessMouseDragEvent(window, xoffset, yoffset);
	}
	else
	{
		g_LastX = xpos;
		g_LastY = ypos;
	}
}

void MouseEvent_Scroll_Callback(GLFWwindow* window, double xpos, double ypos)
{
	const auto& io = ImGui::GetIO();

	if (io.WantCaptureMouse || io.WantCaptureKeyboard)
		return;

	for (const auto& it : *g_EventList)
		it->ProcessMouseScroll(window, xpos, ypos);
}

void CEvent::Initialize()
{
	glfwSetMouseButtonCallback(g_Window, MouseEvent_Button_Callback);
	glfwSetCursorPosCallback(g_Window, MouseEvent_Callback);
	glfwSetScrollCallback(g_Window, MouseEvent_Scroll_Callback);
}

