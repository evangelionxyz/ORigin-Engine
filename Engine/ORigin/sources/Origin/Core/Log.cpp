// Copyright (c) 2022-present Evangelion Manuhutu | ORigin Engine

#include "pch.h"
#include "Origin/Core/Log.h"

namespace origin
{
	static Log *s_Instance = nullptr;
	Log::Log()
	{
		s_Instance = this;
	}

	Log* Log::GetInstance()
	{
		return s_Instance;
	}

	void Log::PrintFormattedMessage(const LogLevel level, const std::string& message)
	{
		PrintMessage(level, message);
	}

	void Log::PrintMessage(LogLevel level, const std::string& message)
	{
		printf("%s\n", message.c_str());
	}
}
