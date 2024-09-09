// Copyright (c) 2022-present Evangelion Manuhutu | ORigin Engine

#ifndef UTILS_H
#define UTILS_H

#include "imgui.h"
#include <string>

namespace origin::Utils
{
	static void CenteredText(const char* text)
	{
		const auto windowWidth = ImGui::GetWindowSize().x;
		const auto textWidth = ImGui::CalcTextSize(text).x;
		ImGui::SetCursorPosX((windowWidth - textWidth) * 0.5f);
		ImGui::Text("%s", text);
	}

	static void CenteredWrappedText(const char* fmt, ...)
	{
		const auto windowWidth = ImGui::GetWindowSize().x;
		const auto textWidth = ImGui::CalcTextSize(fmt).x;
		ImGui::SetCursorPosX((windowWidth - textWidth) * 0.5f);

		va_list args;
		va_start(args, fmt);
		ImGui::TextWrappedV(fmt, args);
		va_end(args);
	}

    static void ExecuteScript(const std::string &scriptPath)
    {
        if (std::filesystem::exists(scriptPath))
        {
            std::string absolutePath = std::filesystem::absolute(scriptPath).string();
#ifdef _WIN32
            std::string expandedCmd = "\"" + absolutePath + "\"";
#elif __linux__
            std::string expandedCmd = "bash " + absolutePath;
#endif
            std::system(expandedCmd.c_str());
        }
    }
}

#endif
