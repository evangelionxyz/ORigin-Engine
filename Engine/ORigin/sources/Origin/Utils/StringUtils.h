// Copyright (c) 2022-present Evangelion Manuhutu | ORigin Engine

#ifndef STRING_UTILS_H
#define STRING_UTILS_H

#include <string>

namespace origin::Utils
{
	std::string BytesToString(uint64_t bytes);
	std::string CapitalizeText(std::string& text);
	std::string CapitalizeWholeText(std::string text);
}

#endif