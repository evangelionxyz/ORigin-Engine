#pragma once
#include "imgui.h"
#include <string>

namespace origin::Utils
{
	static void CenteredText(const char* text)
	{
		const auto windowWidth = ImGui::GetWindowSize().x;
		const auto textWidth = ImGui::CalcTextSize(text).x;
		ImGui::SetCursorPosX((windowWidth - textWidth) * 0.5f);
		ImGui::Text(text);
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
}
