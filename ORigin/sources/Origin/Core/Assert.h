// Copyright (c) 2022 Evangelion Manuhutu | ORigin Engine

#pragma once
#include "Base.h"

#ifdef OGN_ENABLE_ASSERT
	#define OGN_INTERNAL_ASSERT_IMPL(type, check, msg, ...) { if(!(check)) { ##type##ERROR(msg, __VA_ARGS__); DEBUGBREAK(); } }
	#define OGN_INTERNAL_ASSERT_WITH_MSG(type, check, ...) OGN_INTERNAL_ASSERT_IMPL(type, check, "Assertion Failed", EXPAND_STRINGIFY_MACRO(__VA_ARGS__))
	#define OGN_INTERNAL_ASSERT_NO_MSG(type, check) OGN_INTERNAL_ASSERT_IMPL(type, check, "Assertion ", EXPAND_STRINGIFY_MACRO(check), std::filesystem::path(__FILE__))

	#define OGN_INTERNAL_ASSERT_GET_MACRO_NAME(arg1, arg2, macro, ...) macro
	#define OGN_INTERNAL_ASSERT_GET_MACRO(arg1, arg2, macro, ...) EXPAND_MACRO( OGN_INTERNAL_ASSERT_GET_MACRO_NAME(__VA_ARGS__, OGN_INTERNAL_ASSERT_WITH_MSG, OGN_INTERNAL_ASSERT_NO_MSG) )

	#define OGN_ASSERT(...) EXPAND_MACRO( OGN_INTERNAL_ASSERT_GET_MACRO(__VA_ARGS__)(_, __VA_ARGS__) )
	#define OGN_CORE_ASSERT(...) EXPAND_MACRO( OGN_INTERNAL_ASSERT_GET_MACRO(__VA_ARGS__)(_CORE_, __VA_ARGS__) )
#else
	#define OGN_ASSERT(...)
	#define OGN_CORE_ASSERT(...)
#endif