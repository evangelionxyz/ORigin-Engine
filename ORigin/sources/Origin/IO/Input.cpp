// Copyright (c) 2022 Evangelion Manuhutu | ORigin Engine

#include "pch.h"
#include "Input.h"
#include "Origin\Core\Application.h"

namespace origin
{
	Input* Input::s_Instance = nullptr;

	Input::Input()
	{
		s_Instance = this;
	}

	bool Input::IsKeyPressed(const KeyCode keycode)
	{
		GLFWwindow* window = static_cast<GLFWwindow*>(Application::Get().GetWindow().GetNativeWindow());
		int state = glfwGetKey(window, static_cast<int32_t>(keycode));
		return state == GLFW_PRESS || state == GLFW_REPEAT;
	}

	bool Input::IsMouseButtonPressed(const MouseCode button)
	{
		GLFWwindow* window = static_cast<GLFWwindow*>(Application::Get().GetWindow().GetNativeWindow());
		int state = glfwGetMouseButton(window, static_cast<int32_t>(button));
		s_Instance->m_IsMouseDragging = state == GLFW_PRESS;
		return state == GLFW_PRESS;
	}

	bool Input::IsMouseDragging()
	{
		return s_Instance->m_IsMouseDragging;
	}

	glm::vec2 Input::GetMousePosition()
	{
		GLFWwindow* window = static_cast<GLFWwindow*>(Application::Get().GetWindow().GetNativeWindow());
		double xpos, ypos;
		glfwGetCursorPos(window, &xpos, &ypos);
		return { (float)xpos, (float)ypos };
	}

	float Input::GetMouseX()
	{
		return s_Instance->GetMousePosition().x;
	}

	float Input::GetMouseY()
	{
		return s_Instance->GetMousePosition().y;
	}

	void Input::SetMousePosition(float x, float y)
	{
		GLFWwindow* window = static_cast<GLFWwindow*>(Application::Get().GetWindow().GetNativeWindow());
		glfwSetCursorPos(window, (double)x, (double)y);
	}
}