// Copyright (c) Evangelion Manuhutu | ORigin Engine

#pragma once

#ifdef __linux__
	#define _GLFW_X11
#endif

#include <GLFW/glfw3.h>
#include "Event.h"

#ifdef OGN_PLATFORM_WINDOWS
	#include <Windows.h>
#endif

namespace origin {

	class GraphicsContext;

	struct WindowData
	{
		std::string Title;
		uint32_t Width;
		uint32_t Height;
		int xPos;
		int yPos;
		bool Maximized = false;
		bool VSync = false;
		int Close = 0;
		std::function<void(Event &)> EventCallback;
	};
	
	class Window
	{
	public:
		virtual ~Window() {};

		static void GLFWInit();
		static void GLFWShutdown();

		virtual void OnUpdate() = 0;
		virtual bool IsLooping() = 0;
		virtual void SetFullscreen(bool enable) = 0;
		virtual void SetClose(bool close) = 0;
		virtual void SetVSync(bool enable = false) = 0;
		virtual void SetIcon(const char* filepath) = 0;
		virtual void SetEventCallback(const std::function<void(Event&)>& callback) = 0;

		virtual uint32_t GetWidth() const = 0;
		virtual uint32_t GetHeight() const = 0;
		virtual const char* GetTitle() const = 0;
		virtual void *GetNativeWindow() { return nullptr; }
		static std::shared_ptr<Window> Create(const char *title, uint32_t width, uint32_t heigth, bool maximized = false);
	};
}