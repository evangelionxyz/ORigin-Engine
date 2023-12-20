// Copyright (c) Evangelion Manuhutu | ORigin Engine

#pragma once
#include <memory>

#ifdef OGN_DEBUG
	#define OGN_ENABLE_ASSERTS
	#define OGN_DEBUGBREAK() __debugbreak()

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

#include "Origin\Core\Log.h"
#include "Origin\Core\Assert.h"
