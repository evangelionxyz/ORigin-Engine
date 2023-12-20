// Copyright (c) Evangelion Manuhutu | ORigin Engine

#pragma once
#include "Base.h"
#include "Log.h"

#include <filesystem>

#ifdef OGN_DEBUG
	#define OGN_CORE_ASSERT(check, ...) if(!(check)) { OGN_CORE_ERROR(__VA_ARGS__); OGN_DEBUGBREAK(); }
#else
	#define OGN_CORE_ASSERT(...)
#endif