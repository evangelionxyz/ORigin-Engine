// Copyright (c) 2022-present Evangelion Manuhutu | ORigin Engine

#ifndef LOG_H
#define LOG_H

#include "Origin/Utils/StringUtils.h"
#include <iostream>

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/string_cast.hpp>

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
	Log();
	Log(const Log &) = delete;
	void PrintMessage(const std::string &message, LogLevel level = LogLevel::Info)
	{
		PrintColoredMessage(message, level);
	}
	template<typename... Args>
	void PrintMessage(LogLevel level, const char *format, Args &&...args)
	{
		std::string message = Utils::FormatString(format, std::forward<Args>(args)...);
		PrintMessage(message, level);
	}

	void PrintColoredMessage(const std::string &message, const LogLevel level);

    static Log *GetInstance();

private:
	std::stringstream m_Buffer;
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
#define OGN_CORE_ERROR(format, ...)     Log::GetInstance()->PrintMessage(LogLevel::Error, format, __VA_ARGS__)
#define OGN_CORE_WARN(format, ...)      Log::GetInstance()->PrintMessage(LogLevel::Warning, format, __VA_ARGS__)
#define OGN_CORE_INFO(format, ...)      Log::GetInstance()->PrintMessage(LogLevel::Info, format, __VA_ARGS__)
#define OGN_CORE_TRACE(format, ...)     Log::GetInstance()->PrintMessage(LogLevel::Trace, format, __VA_ARGS__)
#define OGN_CORE_CRITICAL(format, ...)	Log::GetInstance()->PrintMessage(LogLevel::Critical, format, __VA_ARGS__)
#else
#define OGN_CORE_ERROR(format, ...)
#define OGN_CORE_WARN(format, ...)
#define OGN_CORE_INFO(format, ...)
#define OGN_CORE_TRACE(format, ...)
#define OGN_CORE_CRITICAL(format, ...)
#endif

#endif