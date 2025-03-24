// Copyright (c) Evangelion Manuhutu | ORigin Engine

#include "pch.h"
#include "Input.h"
#include "KeyCodes.h"
#include "MouseCodes.h"
#include "Origin/Core/Application.h"

namespace origin {

glm::ivec2 Input::mouse_position = glm::ivec2(0);
glm::ivec2 Input::click_delta_position = glm::ivec2(0);
CursorMode Input::cursor_mode = CursorMode::Default;
std::unordered_map<KeyModCode, bool> Input::key_mod_codes;
std::unordered_map<KeyCode, bool> Input::key_codes;
std::unordered_map<MouseCode, bool> Input::mouse_codes;
SDLWindow *Input::window = nullptr;

void Input::Init(SDLWindow *window)
{
	Input::window = window;
}

bool Input::IsKeyReleased(const KeyCode keycode)
{
    return !key_codes[keycode];
}

bool Input::IsKeyPressed(const KeyCode keycode)
{
    return key_codes[keycode];
}

bool Input::IsKeyModReleased(const KeyModCode keymod)
{
    return !key_mod_codes[keymod];
}

bool Input::IsKeyModPressed(const KeyModCode keymod)
{
    return key_mod_codes[keymod];
}

bool Input::IsMouseButtonPressed(const MouseCode button)
{
    return mouse_codes[button];
}

void Input::SetKey(SDL_Keycode key, bool pressed)
{
    key_codes[key] = pressed;
}

void Input::SetModifier(SDL_Keymod mod, bool pressed)
{
    key_mod_codes[mod] = pressed;
}

i32 Input::GetMouseX()
{
    return GetMousePosition().x;
}
i32 Input::GetMouseY()
{
    return GetMousePosition().y;
}

void Input::SetMousePosition(i32 x, i32 y)
{
    SDL_WarpMouseInWindow(window->GetNativeWindow(), static_cast<f32>(x), static_cast<f32>(y));
}

glm::ivec2 Input::GetMousePosition()
{
    f32 x, y;
    SDL_GetMouseState(&x, &y);
    return { static_cast<f32>(x), static_cast<f32>(y) };
}

glm::ivec2 Input::GetMouseClickDragDelta()
{
	return click_delta_position;
}

void Input::Update()
{
    mouse_position = GetMousePosition();

	static glm::ivec2 last_mouse_position = mouse_position;

	if (IsAnyMouseDown())
	{
        click_delta_position = last_mouse_position - mouse_position;
	}
	else
	{
		click_delta_position = { 0, 0 };
	}

    last_mouse_position = mouse_position;
}

void Input::ToggleMouseLock()
{
	cursor_mode = cursor_mode != CursorMode::Lock ? CursorMode::Lock : CursorMode::Default;
	SetCursoreMode(cursor_mode);
}

void Input::SetCursoreMode(CursorMode mode)
{
	cursor_mode = mode;
    switch (mode)
    {
    case CursorMode::Default:
    {
		SDL_ShowCursor();
        break;
    }
    case CursorMode::Lock:
    case CursorMode::Hidden:
    case CursorMode::Captured:
    {
        SDL_HideCursor();
        break;
    }
    }
}

CursorMode Input::GetCursorMode()
{
	return cursor_mode;
}

bool Input::IsAnyMouseDown()
{
	return IsMouseButtonPressed(Mouse::ButtonLeft) || IsMouseButtonPressed(Mouse::ButtonRight) || IsMouseButtonPressed(Mouse::ButtonMiddle);
}

void Input::SetMouseToCenter()
{
	f32 width = static_cast<f32>(window->GetWidth());
	f32 height = static_cast<f32>(window->GetHeight());
	SDL_WarpMouseInWindow(window->GetNativeWindow(), width / 2.0f, height / 2.0f);
}

}