// Copyright (c) 2022 Evangelion Manuhutu | ORigin Engine

#pragma once
#include "pch.h"
#include "KeyCodes.h"
#include "MouseCodes.h"

#include <glm\glm.hpp>

namespace origin
{
	class Input
	{
	public:
		Input();

		static bool IsKeyReleased(const KeyCode keycode);
		static bool IsKeyPressed(const KeyCode keycode);
		static bool IsMouseButtonPressed(const MouseCode button);
		static bool IsMouseDragging();

		static glm::vec2 GetMousePosition();
		static float GetMouseX();
		static float GetMouseY();
		static void SetMousePosition(float x, float y);

	private:
		bool m_IsMouseDragging = false;
		glm::vec2 m_MouseInitialPosition = glm::vec2(0.0f);
		static Input* s_Instance;
	};
}