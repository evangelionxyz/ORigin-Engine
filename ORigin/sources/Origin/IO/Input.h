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

		bool IsKeyPressed(const KeyCode keycode);
		bool IsMouseButtonPressed(const MouseCode button);
		bool IsMouseDragging();

		const glm::vec2 GetDeltaMouse();
		glm::vec2 GetMousePosition();
		float GetMouseX();
		float GetMouseY();
		void SetMousePosition(float x, float y);

		static Input& Get() { return *s_Instance; }

	private:
		bool m_IsMouseDragging = false;
		glm::vec2 m_MouseInitialPosition = glm::vec2(0.0f);
		static Input* s_Instance;
	};
}