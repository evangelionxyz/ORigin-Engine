// Copyright (c) Evangelion Manuhutu | ORigin Engine

#include "pch.h"
#include "Input.h"
#include "KeyCodes.h"
#include "MouseCodes.h"
#include "Origin/Core/Application.h"

namespace origin
{
	Input *Input::s_Instance = nullptr;
	GLFWwindow *s_Window = nullptr;

	Input::Input()
	{
	}

	void Input::Init(void *window)
	{
		s_Window = (GLFWwindow *)window;
		s_Instance = this;
	}

	bool Input::IsKeyReleased(const KeyCode keycode)
	{
		OGN_PROFILER_INPUT();

		int state = glfwGetKey(s_Window, static_cast<int32_t>(keycode));
		return state == GLFW_RELEASE;
	}

	bool Input::IsKeyPressed(const KeyCode keycode)
	{
		OGN_PROFILER_INPUT();

		int state = glfwGetKey(s_Window, static_cast<int32_t>(keycode));
		return state == GLFW_PRESS;
	}

	bool Input::IsMouseButtonPressed(const MouseCode button)
	{
		OGN_PROFILER_INPUT();

		int state = glfwGetMouseButton(s_Window, static_cast<int32_t>(button));
		m_IsMouseDragging = state == GLFW_PRESS;
		return state == GLFW_PRESS;
	}

	glm::vec2 Input::GetMousePosition() const
	{
		OGN_PROFILER_INPUT();

		double xpos, ypos;
		glfwGetCursorPos(s_Window, &xpos, &ypos);
		return { (float)xpos, (float)ypos };
	}

    glm::vec2 Input::GetMouseDelta() const
    {
		return m_MouseDelta;
    }

    void *Input::GetWindow()
    {
		return s_Window;
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
		glfwSetCursorPos(s_Window, (double)x, (double)y);
	}

    void Input::SetMouseLastPosition(float x, float y)
    {
		m_LastMousePosition = { x, y };
    }

    void Input::Update()
	{
        // Update mouse delta
        glm::vec2 currentMousePos = GetMousePosition();
        m_MouseDelta = currentMousePos - m_LastMousePosition;
        m_LastMousePosition = currentMousePos;
	}

	void Input::ToggleMouseLock()
	{
		m_IsMouseLocked = !m_IsMouseLocked;

		if (m_IsMouseLocked)
		{
			glfwSetInputMode(s_Window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
			m_IsMouseHidden = true;
		}
		else
		{
			glfwSetInputMode(s_Window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
			m_IsMouseHidden = false;
		}
	}

    void Input::MouseHide()
    {
		if (!m_IsMouseHidden)
		{
			glfwSetInputMode(s_Window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
			m_IsMouseHidden = true;
		}
    }

    void Input::MouseUnHide()
    {
		if (m_IsMouseHidden)
		{
			glfwSetInputMode(s_Window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
			m_IsMouseHidden = false;
		}
    }

    void Input::SetMouseHide(bool hide)
    {
		hide ? MouseHide() : MouseUnHide();
    }

    bool Input::IsHidden() const
    {
		return m_IsMouseHidden;
    }

    bool Input::IsDragging() const
    {
		return m_IsMouseDragging;
    }

    void Input::SetMouseToCenter()
    {
        int width, height;
        glfwGetWindowSize(s_Window, &width, &height);
        SetMousePosition(width / 2.0f, height / 2.0f);
		m_LastMousePosition = { width / 2.0f, height / 2.0f };
    }

    void Input::ResetMouseDelta()
    {
		m_MouseDelta = { 0.0f, 0.0f };
		m_LastMousePosition = { 0.0f, 0.0f };
    }

    Input &Input::Get()
    {
		return *s_Instance;
    }

}