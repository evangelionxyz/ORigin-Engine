// Copyright (c) 2022-present Evangelion Manuhutu | ORigin Engine

#ifndef LOG_H
#define LOG_H

#include "Origin/Utils/StringUtils.h"
#include "UUID.h"

#include <iostream>
#include <filesystem>

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/string_cast.hpp>

#include <spdlog/spdlog.h>
#include <spdlog/async.h>
#include <spdlog/sinks/stdout_color_sinks.h>

namespace origin {

enum LogLevel
{
    Info = 0,
    Warning,
    Error,
    Trace,
    Critical
};

class Log
{
public:
	static void Init();
	static void Shutdown();

	inline static Ref<spdlog::async_logger> &GetCoreLogger() { return s_core_logger; }
	inline static Ref<spdlog::async_logger> &GetClientLogger() { return s_client_logger; }

private:
	static Ref<spdlog::async_logger> s_core_logger;
    static Ref<spdlog::async_logger> s_client_logger;
	static Ref<spdlog::sinks::stdout_color_sink_mt> s_console_sink;
};

}

namespace fmt {

template<> 
struct formatter<origin::UUID>
{
    // parse the format specification (e.g, "{}" or "{:x}" for hex)
    template<typename ParseContext>
    constexpr auto parse(ParseContext &ctx)
    {
        return ctx.begin();
    }

    // format the UUD (convert to u64 and format it)
    template<typename FormatContext>
    auto format(const origin::UUID &uuid, FormatContext &ctx) const 
    {
        return fmt::format_to(ctx.out(), "{}", static_cast<u64>(uuid));
    }
};

template<>
struct formatter<std::filesystem::path>
{
    template<typename ParseContext>
    constexpr auto parse(ParseContext &ctx)
    {
        return ctx.begin();
    }

    template<typename FormatContext>
    auto format(const std::filesystem::path &filepath, FormatContext &ctx) const
    {
        return fmt::format_to(ctx.out(), "{}", filepath.generic_string());
    }
};

}

template<typename OStream, glm::length_t L, typename T, glm::qualifier Q>
OStream& operator<<(OStream& os, const glm::vec<L, T, Q>& vector)
{
	return os << glm::to_string(vector);
}

template<typename OStream, glm::length_t C, glm::length_t R, typename T, glm::qualifier Q>
OStream& operator<<(OStream& os, const glm::mat<C, R, T, Q>& matrix)
{
	return os << glm::to_string(matrix);
}

template<typename OStream, typename T, glm::qualifier Q>
OStream& operator<<(OStream& os, glm::qua<T, Q> quaternion)
{
	return os << glm::to_string(quaternion);
}

#ifdef OGN_PLATFORM_WINDOWS
// engine logger
#define OGN_CORE_ERROR(...)     Log::GetCoreLogger()->error(__VA_ARGS__)
#define OGN_CORE_WARN(...)      Log::GetCoreLogger()->warn(__VA_ARGS__)
#define OGN_CORE_INFO(...)      Log::GetCoreLogger()->info(__VA_ARGS__)
#define OGN_CORE_TRACE(...)     Log::GetCoreLogger()->trace(__VA_ARGS__)
#define OGN_CORE_CRITICAL(...)  Log::GetCoreLogger()->critical(__VA_ARGS__)

// client logger
#define LOG_ERROR(...)     Log::GetClientLogger()->error(__VA_ARGS__)
#define LOG_WARN(...)      Log::GetClientLogger()->warn(__VA_ARGS__)
#define LOG_INFO(...)      Log::GetClientLogger()->info(__VA_ARGS__)
#define LOG_TRACE(...)     Log::GetClientLogger()->trace(__VA_ARGS__)
#define LOG_CRITICAL(...)  Log::GetClientLogger()->critical(__VA_ARGS__)

#else
#define OGN_CORE_ERROR(...)
#define OGN_CORE_WARN(...)
#define OGN_CORE_INFO(...)
#define OGN_CORE_TRACE(...)
#define OGN_CORE_CRITICAL(...)
#endif

#endif