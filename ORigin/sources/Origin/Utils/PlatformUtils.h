// Copyright (c) 2022 Evangelion Manuhutu | ORigin Engine

#pragma once
#include <string>

namespace origin
{
	class FileDialogs
	{
	public:
		static std::string OpenFile(const char* filter);
		static std::string SaveFile(const char* filter);
	};
}