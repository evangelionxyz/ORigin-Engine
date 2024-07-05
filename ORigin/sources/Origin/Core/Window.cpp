// Copyright (c) Evangelion Manuhutu | ORigin Engine
#include "Window.h"

#ifdef OGN_PLATFORM_WINDOWS
	#include "Platform/Win32/Win32Window.h"
#elif OGN_PLATFORM_LINUX
	#include "Platform/Linux/LinuxWindow.h"
#endif

namespace origin
{
	std::shared_ptr<Window> Window::Create(const char* title,	uint32_t width,	uint32_t height, bool maximized)
	{
#ifdef OGN_PLATFORM_WINDOWS
		return std::make_shared<Win32Window>(title, width, height, maximized);
#elif OGN_PLATFORM_LINUX
		return std::make_shared<LinuxWindow>(title, width, height, maximized);
#else
		OGN_CORE_ASSERT(false, "Unkown Platform");
		return nullptr;
#endif
	}

	void Window::GLFWInit()
	{
		int success = glfwInit();
		if(!success)
		{
			const char *description;
			glfwGetError(&description);
			OGN_CORE_ASSERT(false, "[Window::GLFWInit] Failed to initialize GLFW: {0}", description);
		}
	}

	void Window::GLFWShutdown() 
	{
		glfwTerminate();
	}

}