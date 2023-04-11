// Copyright (c) 2022 Evangelion Manuhutu | ORigin Engine

#pragma once
#include <filesystem>

#ifdef OGN_DEBUG
	#define OGN_DEBUGBREAK __debugbreak();
	#define OGN_ENABLE_ASSERT
#endif

#define EXPAND_MACRO(x)
#define EXPAND_STRINGIFY_MACRO(x) #x

#define BIT(x)(1 << x)
#define OGN_BIND_EVENT_FN(fn) [this](auto&&... args) -> decltype(auto) { return this->fn(std::forward<decltype(args)>(args)...); }

#include "Origin\Core\Log.h"
#include "Origin\Core\Assert.h"