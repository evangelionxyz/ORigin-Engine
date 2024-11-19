// Copyright (c) 2022-present Evangelion Manuhutu | ORigin Engine

#ifndef LOG_H
#define LOG_H

#include "ORigin/Utils/StringUtils.h"
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
	void PrintColoredMessage(const std::string &message, const LogLevel level)
	{
        const char *color_code;
        const char *level_str;
        switch (level)
        {
        case LogLevel::Critical:
        case LogLevel::Error:
            color_code = "\033[1;31m";
            level_str = "[Error]";
            break;
        case LogLevel::Warning:
            color_code = "\033[1;33m";
            level_str = "[Warning]";
            break;
        default:
        	color_code = "\033[1;37m";
        	level_str = "[Info]";
        	break;
        }
        if (m_Buffer.tellp() > 1024)
        {
            m_Buffer.str("");
            m_Buffer.clear();
        }
		// m_Buffer << color_code << level_str << message << "\033[0m\n";
		m_Buffer << message << "\n";
		std::cout << m_Buffer.rdbuf();
		m_Buffer.str("");
		m_Buffer.clear();
	}
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

#define OGN_CORE_ERROR(format, ...)     Log::GetInstance()->PrintMessage(LogLevel::Error, format, __VA_ARGS__)
#define OGN_CORE_WARN(format, ...)      Log::GetInstance()->PrintMessage(LogLevel::Warning, format, __VA_ARGS__)
#define OGN_CORE_INFO(format, ...)      Log::GetInstance()->PrintMessage(LogLevel::Info, format, __VA_ARGS__)
#define OGN_CORE_TRACE(format, ...)     Log::GetInstance()->PrintMessage(LogLevel::Trace, format, __VA_ARGS__)
#define OGN_CORE_CRITICAL(format, ...)	Log::GetInstance()->PrintMessage(LogLevel::Critical, format, __VA_ARGS__)

#endif