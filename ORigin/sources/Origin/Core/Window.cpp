// Copyright (c) 2022 Evangelion Manuhutu | ORigin Engine

#include "pch.h"
#include "Window.h"

#ifdef OGN_WINDOWS_PLATFORM
	#include "Platform/Windows/Win_Window.h"
#endif

namespace Origin
{
	std::unique_ptr<Window> Window::Create(const std::string& title)
	{
#ifdef OGN_WINDOWS_PLATFORM
		return std::make_unique<WinWindow>(title);
#else
		OGN_CORE_ASSERT(false, "Unkown Platform");
		return nullptr;
#endif
	}
}