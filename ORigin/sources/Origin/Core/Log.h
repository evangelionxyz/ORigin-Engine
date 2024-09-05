// Copyright (c) 2022-present Evangelion Manuhutu | ORigin Engine

#ifndef LOG_H
#define LOG_H

#include "LogLevel.h"

#include <fmt/format.h>
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/string_cast.hpp>

#include "Types.h"
#include "UUID.h"
#include <miniaudio.h>
#include <vulkan/vulkan.h>

namespace origin {
	class Log
	{
	public:
		Log();
		Log(const Log &) = delete;

		template<typename ...Args>
		void PrintFormattedMessage(const LogLevel level, fmt::format_string<Args...> format, Args&&... args)
		{
			const std::string message = fmt::format(format, std::forward<Args>(args)...);
			PrintMessage(level, message);
		}

		void PrintFormattedMessage(LogLevel level, const std::string &message);

		void PrintMessage(LogLevel level, const std::string &message);
		static Log *GetInstance();
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


namespace fmt
{
	template<>
	struct formatter<origin::UUID> : formatter<u64>
	{
		auto format(const origin::UUID& uuid, format_context& ctx) const
		{
			return formatter<u64>::format(uuid, ctx);
		}
	};

	template<>
	struct formatter<ma_result> : formatter<u64>
	{
		auto format(const ma_result& result, format_context& ctx) const
		{
			return formatter<u64>::format(static_cast<u64>(result), ctx);
		}
	};

	template<>
	struct formatter<VkResult> : formatter<u64>
	{
		auto format(const VkResult& result, format_context& ctx) const
		{
			return formatter<u64>::format(static_cast<u64>(result), ctx);
		}
	};
}

#define OGN_CORE_ERROR(...)     Log::GetInstance()->PrintFormattedMessage(LogLevel::Error, __VA_ARGS__)
#define OGN_CORE_WARN(...)      Log::GetInstance()->PrintFormattedMessage(LogLevel::Warning, __VA_ARGS__)
#define OGN_CORE_INFO(...)      Log::GetInstance()->PrintFormattedMessage(LogLevel::Info, __VA_ARGS__)
#define OGN_CORE_TRACE(...)     Log::GetInstance()->PrintFormattedMessage(LogLevel::Trace, __VA_ARGS__)
#define OGN_CORE_CRITICAL(...)	Log::GetInstance()->PrintFormattedMessage(LogLevel::Critical, __VA_ARGS__)

#endif