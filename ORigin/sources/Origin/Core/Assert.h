// Copyright (c) Evangelion Manuhutu | ORigin Engine

#pragma once
#include "Base.h"
#include "Log.h"

#include <filesystem>

#ifdef OGN_ENABLE_ASSERTS
	#define OGN_INTERNAL_ASSERT_IMPL(type, check, msg, ...) { if(!(check)) { OGN##type##ERROR(msg, __VA_ARGS__); OGN_DEBUGBREAK(); }}
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

#ifdef OGN_ENABLE_VERIFY
	#define OGN_INTERNAL_VERIFY_IMPL(type, check, msg, ...) { if(!(check)) { OGN##type##ERROR(msg, __VA_ARGS__); OGN_DEBUGBREAK(); } }
	#define OGN_INTERNAL_VERIFY_WITH_MSG(type, check, ...) OGN_INTERNAL_VERIFY_IMPL(type, check, "Assertion Failed", EXPAND_STRINGIFY_MACRO(__VA_ARGS__))
	#define OGN_INTERNAL_VERIFY_NO_MSG(type, check) OGN_INTERNAL_VERIFY_IMPL(type, check, "Assertion ", EXPAND_STRINGIFY_MACRO(check), std::filesystem::path(__FILE__))

	#define OGN_INTERNAL_VERIFY_GET_MACRO_NAME(arg1, arg2, macro, ...) macro
	#define OGN_INTERNAL_VERIFY_GET_MACRO(arg1, arg2, macro, ...) EXPAND_MACRO( OGN_INTERNAL_VERIFY_GET_MACRO_NAME(__VA_ARGS__, OGN_INTERNAL_VERIFY_WITH_MSG, OGN_INTERNAL_VERIFY_NO_MSG) )

	#define OGN_VERIFY(...) EXPAND_MACRO( OGN_INTERNAL_VERIFY_GET_MACRO(__VA_ARGS__)(_, __VA_ARGS__) )
	#define OGN_CORE_VERIFY(...) EXPAND_MACRO( OGN_INTERNAL_VERIFY_GET_MACRO(__VA_ARGS__)(_CORE_, __VA_ARGS__) )
	#else
	#define OGN_VERIFY(...)
	#define OGN_CORE_VERIFY(...)
#endif