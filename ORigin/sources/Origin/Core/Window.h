// Copyright (c) 2022 Evangelion Manuhutu | ORigin Engine

#pragma once
#include <GLFW/glfw3.h>
#include "Origin/IO/Events/Event.h"

#include "glm\glm.hpp"

namespace origin {

	class Window
	{
	public:
		virtual ~Window() {};
		static void GLFWInit();
		static void GLFWShutdown();
		virtual void OnUpdate() = 0;
		virtual bool IsLooping() const = 0;
		virtual void SetFullscreen(bool enable) = 0;
		virtual void SetClose(bool close) = 0;
		virtual void SetVSync(bool enable = false) = 0;
		virtual void SetIcon(const char* filepath) = 0;
		virtual void SetEventCallback(const std::function<void(Event&)>& callback) = 0;

		virtual uint32_t GetWidth() const = 0;
		virtual uint32_t GetHeight() const = 0;
		virtual const char* GetTitle() const = 0;
		virtual GLFWwindow* GetNativeWindow() = 0;

		static std::unique_ptr<Window> Create(const char* title, uint32_t width, uint32_t heigth, bool maximized = false);

	};
}