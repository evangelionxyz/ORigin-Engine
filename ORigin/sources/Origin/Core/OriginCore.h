// Copyright (c) 2022 Evangelion Manuhutu | ORigin Engine

#include "Origin\Utils\log.h"

// ORIGIN LOG
#define OGN_CORE_ERROR(...)			::Origin::Log::GetCoreLogger()->error(__VA_ARGS__)
#define OGN_CORE_WARN(...)			::Origin::Log::GetCoreLogger()->warn(__VA_ARGS__)
#define OGN_CORE_INFO(...)			::Origin::Log::GetCoreLogger()->info(__VA_ARGS__)
#define OGN_CORE_TRACE(...)			::Origin::Log::GetCoreLogger()->trace(__VA_ARGS__)
#define OGN_CORE_CRITICAL(...)	::Origin::Log::GetCoreLogger()->critical(__VA_ARGS__)

// SANDBOX LOG
#define OGN_ERROR(...)					::Origin::Log::GetClientLogger()->error(__VA_ARGS__)
#define OGN_WARN(...)						::Origin::Log::GetClientLogger()->warn(__VA_ARGS__)
#define OGN_INFO(...)						::Origin::Log::GetClientLogger()->info(__VA_ARGS__)
#define OGN_TRACE(...)					::Origin::Log::GetClientLogger()->trace(__VA_ARGS__)
#define OGN_CRITICAL(...)				::Origin::Log::GetClientLogger()->critical(__VA_ARGS__)

// ASSERTION
#define OGN_CORE_ASSERT(x, ...) { if(!(x)) { OGN_CORE_ERROR("Assertion Failed: {0}", __VA_ARGS__); __debugbreak(); } }
#define OGN_CORE_ASSERT(x) { if(!(x)) { __debugbreak(); } }

#define OGN_ASSERT(x, ...) { if(!(x)) { OGN_ERROR("Assertion Failed: {0}", __VA_ARGS__); __debugbreak(); } }

#define BIT(x)(1 << x)
#define OGN_BIND_EVENT_FN(fn) [this](auto&&... args) -> decltype(auto) { return this->fn(std::forward<decltype(args)>(args)...); }