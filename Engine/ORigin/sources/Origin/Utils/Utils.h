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

    static void ExecuteScript(const std::string &script_path)
    {
        if (std::filesystem::exists(script_path))
        {
#if defined(__linux__)
            std::string command_str = "bash " + script_path;
            std::system(command_str.c_str());
#elif defined(_WIN32)
            // Use PowerShell to execute the script
            std::string command_str = "pwsh -Command \"& '" + script_path + "'\"";
            std::system(command_str.c_str());
#endif
        }
        else
        {
            throw std::runtime_error("Script file does not exist: " + script_path);
        }
    }
}

#endif
