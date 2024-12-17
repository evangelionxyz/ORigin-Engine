// Copyright (c) 2022 Evangelion Manuhutu | ORigin Engine

#ifndef INPUT_H
#define INPUT_H

#include "KeyCodes.h"
#include "MouseCodes.h"

#include <glm/glm.hpp>

struct GLFWwindow;

namespace origin {

enum CursorMode : u8
{
	Default = BIT(0),
	Lock = BIT(1),
	Hidden = BIT(2),
	Captured = BIT(3),
};

struct InputData
{
    glm::ivec2 mouse_position = glm::ivec2(0);

	// TODO: click delta for individual button
	// using array
    glm::ivec2 click_delta_position = glm::ivec2(0);
	CursorMode cursor_mode = CursorMode::Default;
    GLFWwindow *window = nullptr;
};

class Input
{
public:
	static void Init(GLFWwindow *window);
	static bool IsKeyReleased(const KeyCode keycode);
	static bool IsKeyPressed(const KeyCode keycode);
	static bool IsMouseButtonPressed(const MouseCode button);
	static i32 GetMouseX();
	static i32 GetMouseY();
	static void SetMousePosition(i32 x, i32 y);
	static glm::ivec2 GetMouseClickDragDelta();

	static void Update();
	static void ToggleMouseLock();
	static void SetCursoreMode(CursorMode mode);
	static CursorMode GetCursorMode();
	static bool IsAnyMouseDown();
	static void SetMouseToCenter();
private:
	friend class EditorCamera;
};

}

#endif