// Copyright (c) Evangelion Manuhutu | ORigin Engine

#include "pch.h"
#include "StringUtils.h"

namespace origin::Utils
{
	std::string BytesToString(uint64_t bytes)
	{
		constexpr uint64_t GB = 1024 * 1024 * 1024;
		constexpr uint64_t MB = 1024 * 1024;
		constexpr uint64_t KB = 1024;

		char buffer[32 + 1]{};

		if (bytes >= GB)
			snprintf(buffer, 32, "%.2f GB", (float)bytes / (float)GB);
		else if (bytes >= MB)
			snprintf(buffer, 32, "%.2f MB", (float)bytes / (float)MB);
		else if (bytes >= KB)
			snprintf(buffer, 32, "%.2f KB", (float)bytes / (float)KB);
		else
			snprintf(buffer, 32, "%.2f Bytes", (float)bytes);

		return std::string(buffer);
	}

	std::string CapitalizeText(std::string& text)
	{
		for (int i = 0; i < text.length(); i++)
		{
			if (i == 0) text[i] = (int)std::toupper(text[i]);
			else if (text[i - 1] == ' ')
				text[i] = std::toupper(text[i]);
		}

		return text;
	}

	std::string CapitalizeWholeText(std::string text)
	{
		for (int i = 0; i < static_cast<int>(text.length()); i++)
			text[i] = std::toupper(text[i]);

		return text;
	}
}