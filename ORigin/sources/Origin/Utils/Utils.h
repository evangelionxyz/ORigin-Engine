#pragma once
#include "imgui.h"
#include <string>

namespace origin
{
	namespace Utils
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

		static std::string CapitalizeText(std::string& text)
		{
			for (int i = 0; i < text.length(); i++)
			{
				if (i == 0) text[i] = (int)std::toupper(text[i]);
				else if (text[i - 1] == ' ')
					text[i] = std::toupper(text[i]);
			}

			return text;
		}

		static std::string CapitalizeWholeText(std::string text)
		{
			for (int i = 0; i < static_cast<int>(text.length()); i++)
				text[i] = std::toupper(text[i]);

			return text;
		}
	}
}
