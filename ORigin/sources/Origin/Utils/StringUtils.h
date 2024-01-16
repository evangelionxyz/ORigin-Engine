#pragma once
#include <string>

namespace origin::Utils
{
	std::string BytesToString(uint64_t bytes);
	std::string CapitalizeText(std::string& text);
	std::string CapitalizeWholeText(std::string text);
}