// Copyright (c) Evangelion Manuhutu | ORigin Engine

#include "pch.h"
#include "Input.h"
#include "KeyCodes.h"
#include "MouseCodes.h"
#include "Origin/Core/Application.h"

namespace origin {

static InputData s_InputData;

void Input::Init(GLFWwindow *window)
{
	s_InputData.window = window;
}

bool Input::IsKeyReleased(const KeyCode keycode)
{
	OGN_PROFILER_INPUT();
	const int &state = glfwGetKey(s_InputData.window, static_cast<int32_t>(keycode));
	return state == GLFW_RELEASE;
}

bool Input::IsKeyPressed(const KeyCode keycode)
{
	OGN_PROFILER_INPUT();
	const int &state = glfwGetKey(s_InputData.window, static_cast<int32_t>(keycode));
	return state == GLFW_PRESS;
}

bool Input::IsMouseButtonPressed(const MouseCode button)
{
	OGN_PROFILER_INPUT();
	const int &state = glfwGetMouseButton(s_InputData.window, static_cast<int32_t>(button));
	return state == GLFW_PRESS;
}

i32 Input::GetMouseX()
{
	return s_InputData.mouse_position.x;
}

i32 Input::GetMouseY()
{
	return s_InputData.mouse_position.y;
}

void Input::SetMousePosition(i32 x, i32 y)
{
	glfwSetCursorPos(s_InputData.window, (double)x, (double)y);
}

glm::ivec2 Input::GetMouseClickDragDelta()
{
	return s_InputData.click_delta_position;
}

void Input::Update()
{
	static double x, y;
	glfwGetCursorPos(s_InputData.window, &x, &y);
	s_InputData.mouse_position.x = static_cast<int>(x);
	s_InputData.mouse_position.y = static_cast<int>(y);

	static glm::ivec2 last_mouse_position = s_InputData.mouse_position;

	if (IsAnyMouseDown())
	{
        s_InputData.click_delta_position = last_mouse_position - s_InputData.mouse_position;
	}
	else
	{
		s_InputData.click_delta_position = { 0, 0 };
	}

    last_mouse_position = s_InputData.mouse_position;
}

void Input::ToggleMouseLock()
{
	s_InputData.cursor_mode = s_InputData.cursor_mode != CursorMode::Lock ? CursorMode::Lock : CursorMode::Default;
	SetCursoreMode(s_InputData.cursor_mode);
}

void Input::SetCursoreMode(CursorMode mode)
{
	s_InputData.cursor_mode = mode;
    switch (mode)
    {
    case CursorMode::Default:
    {
        glfwSetInputMode(s_InputData.window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
        break;
    }
    case CursorMode::Lock:
    {
        glfwSetInputMode(s_InputData.window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
        break;
    }
    case CursorMode::Hidden:
    {
        glfwSetInputMode(s_InputData.window, GLFW_CURSOR, GLFW_CURSOR_HIDDEN);
        break;
    }
    case CursorMode::Captured:
    {
        glfwSetInputMode(s_InputData.window, GLFW_CURSOR, GLFW_CURSOR_CAPTURED);
        break;
    }
    }
}

CursorMode Input::GetCursorMode()
{
	return s_InputData.cursor_mode;
}

bool Input::IsAnyMouseDown()
{
	return IsMouseButtonPressed(Mouse::Button0) || IsMouseButtonPressed(Mouse::Button1)
		|| IsMouseButtonPressed(Mouse::Button2) || IsMouseButtonPressed(Mouse::Button3) 
		|| IsMouseButtonPressed(Mouse::Button4) || IsMouseButtonPressed(Mouse::Button5)
		|| IsMouseButtonPressed(Mouse::Button6) || IsMouseButtonPressed(Mouse::Button7);
}

void Input::SetMouseToCenter()
{
	int width, height;
	glfwGetWindowSize(s_InputData.window, &width, &height);
	SetMousePosition(width / 2, height / 2);
}

}