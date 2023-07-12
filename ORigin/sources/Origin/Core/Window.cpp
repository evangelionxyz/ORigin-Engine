// Copyright (c) 2022 Evangelion Manuhutu | ORigin Engine

#include "pch.h"
#include "Window.h"

#ifdef OGN_WINDOWS_PLATFORM
	#include "Platform/Windows/Win_Window.h"
#endif

namespace origin
{
	std::unique_ptr<Window> Window::Create(const std::string& title, uint32_t width, uint32_t height)
	{
#ifdef OGN_WINDOWS_PLATFORM
		return std::make_unique<WinWindow>(title, width, height);
#else
		OGN_CORE_ASSERT(false, "Unkown Platform");
		return nullptr;
#endif
	}
	std::unique_ptr<Window> Window::Create(const WindowConfig& config)
	{
#ifdef OGN_WINDOWS_PLATFORM
		return std::make_unique<WinWindow>(config);
#else
		OGN_CORE_ASSERT(false, "Unkown Platform");
		return nullptr;
#endif
	}

	void Window::GLFWInit()
	{
		int success = glfwInit();
		OGN_CORE_ASSERT(success, "Failed to initialize GLFW");
	}

	void Window::GLFWShutdown() 
	{
		glfwTerminate();
	}
}