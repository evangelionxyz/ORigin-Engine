// Copyright (c) 2022-present Evangelion Manuhutu | ORigin Engine

#ifndef PLATFORM_UTILS_H
#define PLATFORM_UTILS_H

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

#endif