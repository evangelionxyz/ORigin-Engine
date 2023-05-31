// Copyright (c) 2022 Evangelion Manuhutu | ORigin Engine

#pragma once

#define GLM_ENABLE_EXEPERIMENTAL
#include <glm\gtx\string_cast.hpp>

#pragma warning(push, 0)
#include "spdlog\spdlog.h"
#include "spdlog\fmt\ostr.h"
#pragma warning(pop)

namespace origin {

	class Log
	{
	public:
		static void Init();
		inline static std::shared_ptr<spdlog::logger>& GetCoreLogger() { return s_CoreLogger; }
		inline static std::shared_ptr<spdlog::logger>& GetClientLogger() { return s_ClientLogger; }
	private:
		static std::shared_ptr<spdlog::logger> s_CoreLogger;
		static std::shared_ptr<spdlog::logger> s_ClientLogger;
	};
}

template<typename OStream, glm::length_t L, typename T, glm::qualifier Q>
inline OStream& operator<<(OStream& os, const glm::vec<L, T, Q>& vector)
{
	return os << glm::to_string(vector);
}

template<typename OStream, glm::length_t C, glm::length_t R, typename T, glm::qualifier Q>
inline OStream& operator<<(OStream& os, const glm::mat<C, R, T, Q>& matrix)
{
	return os << glm::to_string(matrix);
}

template<typename OStream, typename T, glm::qualifier Q>
inline OStream& operator<<(OStream& os, glm::qua<T, Q> quaternion)
{
	return os << glm::to_string(quaternion);
}

// ORIGIN LOG
#define OGN_CORE_ERROR(...)			::origin::Log::GetCoreLogger()->error(__VA_ARGS__)
#define OGN_CORE_WARN(...)			::origin::Log::GetCoreLogger()->warn(__VA_ARGS__)
#define OGN_CORE_INFO(...)			::origin::Log::GetCoreLogger()->info(__VA_ARGS__)
#define OGN_CORE_TRACE(...)			::origin::Log::GetCoreLogger()->trace(__VA_ARGS__)
#define OGN_CORE_CRITICAL(...)	::origin::Log::GetCoreLogger()->critical(__VA_ARGS__)

// SANDBOX LOG
#define OGN_ERROR(...)					::origin::Log::GetClientLogger()->error(__VA_ARGS__)
#define OGN_WARN(...)						::origin::Log::GetClientLogger()->warn(__VA_ARGS__)
#define OGN_INFO(...)						::origin::Log::GetClientLogger()->info(__VA_ARGS__)
#define OGN_TRACE(...)					::origin::Log::GetClientLogger()->trace(__VA_ARGS__)
#define OGN_CRITICAL(...)				::origin::Log::GetClientLogger()->critical(__VA_ARGS__)