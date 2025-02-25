// Copyright (c) 2022 Evangelion Manuhutu | ORigin Engine

#ifndef INPUT_H
#define INPUT_H

#include "KeyCodes.h"
#include "MouseCodes.h"
#include "SDLWindow.hpp"

#include <unordered_map>

namespace origin {

enum CursorMode : u8
{
	Default = BIT(0),
	Lock = BIT(1),
	Hidden = BIT(2),
	Captured = BIT(3),
};

class Input
{
public:
	static void Init(SDLWindow *window);
	static bool IsKeyReleased(const KeyCode keycode);
	static bool IsKeyPressed(const KeyCode keycode);
	static bool IsMouseButtonPressed(const MouseCode button);
	static i32 GetMouseX();
	static i32 GetMouseY();
	static void SetMousePosition(i32 x, i32 y);
	static glm::ivec2 GetMousePosition();
	static glm::ivec2 GetMouseClickDragDelta();

	static void Update();
	static void ToggleMouseLock();
	static void SetCursoreMode(CursorMode mode);
	static CursorMode GetCursorMode();
	static bool IsAnyMouseDown();
	static void SetMouseToCenter();

    static glm::ivec2 mouse_position;
    static glm::ivec2 click_delta_position;
    static CursorMode cursor_mode;
    static std::unordered_map<KeyCode, bool> key_codes;
    static std::unordered_map<MouseCode, bool> mouse_codes;

private:
    static SDLWindow *window;
	friend class EditorCamera;
};

}

#endif