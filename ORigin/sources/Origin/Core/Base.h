// Copyright (c) Evangelion Manuhutu | ORigin Engine

#pragma once
#include <memory>

#ifdef _WIN32
	#ifndef OGN_PLATFORM_WINDOWS
		#define OGN_PLATFORM_WINDOWS
	#endif
#elif __linux__
	#ifndef OGN_PLATFORM_LINUX
		#define OGN_PLATFORM_LINUX
	#endif
#endif

#ifdef OGN_DEBUG
	#define OGN_ENABLE_ASSERTS
	#ifdef _WIN32
		#define OGN_DEBUGBREAK() __debugbreak()
	#elif __linux__
		#define OGN_DEBUGBREAK() __builtin_trap();
	#endif
#else
	#define OGN_DEBUGBREAK()
#endif

#ifndef OGN_RELEASE
	#define OGN_ENABLE_VERIFY
#endif

#define OGN_EXPAND_MACRO(x)
#define OGN_STRINGIFY_MACRO(x) #x

#define BIT(x)(1 << x)
#define OGN_BIND_EVENT_FN(fn) [this](auto&&... args) -> decltype(auto) { return this->fn(std::forward<decltype(args)>(args)...); }

#include "Origin/Core/Log.h"
#include "Origin/Core/Assert.h"

