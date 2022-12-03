// Copyright (c) 2022 Evangelion Manuhutu | ORigin Engine

#pragma once

#pragma warning(push, 0)
#include "spdlog\spdlog.h"
#include "spdlog\fmt\ostr.h"
#pragma warning(pop)

#include "Origin\Core\OriginCore.h"

namespace Origin {

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