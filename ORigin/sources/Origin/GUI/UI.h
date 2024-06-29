// Copyright (c) Evangelion Manuhutu | ORigin Engine

#pragma once
#include <imgui.h>
#include <imgui_internal.h>
#include <glm\glm.hpp>

#include <functional>

namespace origin::UI
{
	// Enumeration for PushStyleColor() / PopStyleColor()
	enum ColorStyle
	{
		Text,
		TextDisabled,
		WindowBg,              // Background of normal windows
		ChildBg,               // Background of child windows
		PopupBg,               // Background of popups, menus, tooltips windows
		Border,
		BorderShadow,
		FrameBg,               // Background of checkbox, radio button, plot, slider, text input
		FrameBgHovered,
		FrameBgActive,
		TitleBg,               // Title bar
		TitleBgActive,         // Title bar when focused
		TitleBgCollapsed,      // Title bar when collapsed
		MenuBarBg,
		ScrollbarBg,
		ScrollbarGrab,
		ScrollbarGrabHovered,
		ScrollbarGrabActive,
		CheckMark,             // Checkbox tick and RadioButton circle
		SliderGrab,
		SliderGrabActive,
		Button,
		ButtonHovered,
		ButtonActive,
		Header,                // Header* colors are used for CollapsingHeader, TreeNode, Selectable, MenuItem
		HeaderHovered,
		HeaderActive,
		Separator,
		SeparatorHovered,
		SeparatorActive,
		ResizeGrip,            // Resize grip in lower-right and lower-left corners of windows.
		ResizeGripHovered,
		ResizeGripActive,
		TabHovered,            // Tab background, when hovered
		Tab,                   // Tab background, when tab-bar is focused & tab is unselected
		TabSelected,           // Tab background, when tab-bar is focused & tab is selected
		TabSelectedOverline,   // Tab horizontal overline, when tab-bar is focused & tab is selected
		TabDimmed,             // Tab background, when tab-bar is unfocused & tab is unselected
		TabDimmedSelected,     // Tab background, when tab-bar is unfocused & tab is selected
		TabDimmedSelectedOverline,//..horizontal overline, when tab-bar is unfocused & tab is selected
		DockingPreview,        // Preview overlay color when about to docking something
		DockingEmptyBg,        // Background color for empty node (e.g. CentralNode with no window docked into it)
		PlotLines,
		PlotLinesHovered,
		PlotHistogram,
		PlotHistogramHovered,
		TableHeaderBg,         // Table header background
		TableBorderStrong,     // Table outer and header borders (prefer using Alpha=1.0 here)
		TableBorderLight,      // Table inner borders (prefer using Alpha=1.0 here)
		TableRowBg,            // Table row background (even rows)
		TableRowBgAlt,         // Table row background (odd rows)
		TextSelectedBg,
		DragDropTarget,        // Rectangle highlighting a drop target
		NavHighlight,          // Gamepad/keyboard: current highlighted item
		NavWindowingHighlight, // Highlight window when using CTRL+TAB
		NavWindowingDimBg,     // Darken/colorize entire screen behind the CTRL+TAB window list, when active
		ModalWindowDimBg,      // Darken/colorize entire screen behind a modal window, when one is active
		COUNT
	};

	struct ColorStyleVarData
	{
		ColorStyle Style;
		glm::vec4 Color;

		ColorStyleVarData(ColorStyle style, const glm::vec4 &color)
			: Style(style), Color(color) { }
	};

	class PushScopedColorStyle
	{
	public:
		PushScopedColorStyle(std::initializer_list<ColorStyleVarData> styles)
			: m_Styles(styles)
		{
			for(auto s : m_Styles)
				ImGui::PushStyleColor((ImGuiCol)s.Style, ImVec4(s.Color.x, s.Color.y, s.Color.z, s.Color.w));
		}
		~PushScopedColorStyle()
		{
			ImGui::PopStyleColor(m_Styles.size());
		}

	private:
		std::vector<ColorStyleVarData> m_Styles;
	};

	static float defColWidth = 100.0f;

	static bool DrawButton(const char *text, bool *value = nullptr)
	{
		bool ret = false;

		ImGui::PushID((void *)(uint32_t)&text);

		float lineHeight = GImGui->FontSize + GImGui->Style.FramePadding.y * 2.0f;
		float lineWidth = GImGui->FontSize + GImGui->Style.FramePadding.x * (ImGui::CalcTextSize(text).x / 4.0f);
		ImVec2 btSize = ImVec2(lineWidth, lineHeight);

		ret = ImGui::Button(text, btSize);
		if (value)
			*value = ret;

		ImGui::PopID();

		return ret;
	}

	static bool DrawButtonWithColumn(const char *lable, const char *text, bool *value = nullptr, std::function<void()> func = std::function<void()>(), float coloumnWidth = defColWidth)
	{
		bool ret = false;

		ImGui::PushID(lable);

		float lineHeight = GImGui->FontSize + GImGui->Style.FramePadding.y * 2.0f;
		float lineWidth = GImGui->FontSize + GImGui->Style.FramePadding.x * (ImGui::CalcTextSize(text).x / 4.0f);
		ImVec2 btSize = ImVec2(lineWidth, lineHeight);

		ImGui::BeginColumns(lable, 2, ImGuiOldColumnFlags_GrowParentContentsSize);
		ImGui::SetColumnWidth(0, coloumnWidth);
		ImGui::Text(lable);
		ImGui::NextColumn();

		ret = ImGui::Button(text, btSize);

		if(func != nullptr)
			func();

		if(value)
			*value = ret;

		ImGui::EndColumns();
		ImGui::PopID();

		return ret;
	}

	static bool DrawCheckbox(const char *lable, bool *value, float coloumnWidth = defColWidth)
	{
		bool ret = false;

		ImGui::PushID(lable);

		ImGui::BeginColumns(lable, 2, ImGuiOldColumnFlags_GrowParentContentsSize);
		ImGui::SetColumnWidth(0, coloumnWidth);
		ImGui::Text(lable);
		ImGui::NextColumn();

		ret = ImGui::Checkbox("##check_box", value);
		ImGui::EndColumns();
		ImGui::PopID();

		return ret;
	}

	static bool DrawVec4Control(const char *lable, glm::vec4 &values, float speed = 0.025f, float resetValue = 0.0f, float coloumnWidth = defColWidth)
	{
		bool changed = false;
		ImGui::PushID(lable);

		ImGui::Columns(2);
		ImGui::SetColumnWidth(0, coloumnWidth);
		ImGui::Text(lable);
		ImGui::NextColumn();

		ImGui::PushMultiItemsWidths(4, ImGui::CalcItemWidth());
		ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0, 5));

		float lineHeight = GImGui->FontSize + GImGui->Style.FramePadding.y * 2.0f;
		ImVec2 buttonSize = ImVec2(lineHeight, lineHeight);

		// ================================
		// X
		{
			PushScopedColorStyle buttonStyle({
				{ ColorStyle::Button,          { 0.8f, 0.1f, 0.1f, 1.0f } },
				{ ColorStyle::ButtonHovered,   { 0.9f, 0.3f, 0.3f, 1.0f } },
				{ ColorStyle::ButtonActive,    { 0.8f, 0.1f, 0.1f, 1.0f } }
			});

			if (ImGui::Button("X", buttonSize))
			{
				values.x = resetValue;
				changed = true;
			}

			ImGui::SameLine();

			if (ImGui::DragFloat("##X", &values.x, speed))
				changed = true;

			ImGui::PopItemWidth();
			ImGui::SameLine();
		}

		// ================================
		// Y
		{
			PushScopedColorStyle buttonStyle({
				{ ColorStyle::Button,          { 0.1f, 0.6f, 0.1f, 1.0f } },
				{ ColorStyle::ButtonHovered,   { 0.3f, 0.8f, 0.3f, 1.0f } },
				{ ColorStyle::ButtonActive,    { 0.1f, 0.8f, 0.1f, 1.0f } }
			});

			if (ImGui::Button("Y", buttonSize))
			{
				values.y = resetValue;
				changed = true;
			}
			ImGui::SameLine();
			if (ImGui::DragFloat("##Y", &values.y, speed))
				changed = true;
			ImGui::PopItemWidth();
			ImGui::SameLine();
		}

		// ================================
		// Z
		{
			PushScopedColorStyle buttonStyle({
				{ ColorStyle::Button,          { 0.1f, 0.1f, 0.8f, 1.0f } },
				{ ColorStyle::ButtonHovered,   { 0.3f, 0.3f, 0.9f, 1.0f } },
				{ ColorStyle::ButtonActive,    { 0.1f, 0.1f, 0.8f, 1.0f } }
			});

			if (ImGui::Button("Z", buttonSize))
			{
				values.z = resetValue;
				changed = true;
			}

			ImGui::SameLine();
			if (ImGui::DragFloat("##Z", &values.z, speed))
				changed = true;
			ImGui::PopItemWidth();
			ImGui::SameLine();
		}

		// ================================
		// W
		{
			PushScopedColorStyle buttonStyle({
				{ ColorStyle::Button,          { 0.8f, 0.8f, 0.1f, 1.0f } },
				{ ColorStyle::ButtonHovered,   { 0.9f, 0.9f, 0.3f, 1.0f } },
				{ ColorStyle::ButtonActive,    { 0.8f, 0.8f, 0.1f, 1.0f } }
			});

			if (ImGui::Button("W", buttonSize))
			{
				values.x = resetValue;
				changed = true;
			}

			ImGui::SameLine();
			if (ImGui::DragFloat("##W", &values.w, speed))
				changed = true;
			ImGui::PopItemWidth();
		}
		
		ImGui::PopStyleVar(1);
		ImGui::Columns(1);
		ImGui::PopID();

		return changed;
	}

	static bool DrawVec3Control(const char *lable, glm::vec3 &values, float speed = 0.025f, float resetValue = 0.0f, float coloumnWidth = defColWidth)
	{
		bool changed = false;

		ImGui::PushID(lable);

		ImGui::Columns(2);
		ImGui::SetColumnWidth(0, coloumnWidth);
		ImGui::Text(lable);
		ImGui::NextColumn();

		ImGui::PushMultiItemsWidths(3, ImGui::CalcItemWidth());
		ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0, 2));

		float lineHeight = GImGui->FontSize + GImGui->Style.FramePadding.y * 2.0f;
		ImVec2 buttonSize = ImVec2(lineHeight, lineHeight);

		// ================================
		// X
		{
			PushScopedColorStyle buttonStyle({
				{ ColorStyle::Button,          { 0.8f, 0.1f, 0.1f, 1.0f } },
				{ ColorStyle::ButtonHovered,   { 0.9f, 0.3f, 0.3f, 1.0f } },
				{ ColorStyle::ButtonActive,    { 0.8f, 0.1f, 0.1f, 1.0f } }
			});

			if (ImGui::Button("X", buttonSize))
			{
				values.x = resetValue;
				changed = true;
			}

			ImGui::SameLine();

			if (ImGui::DragFloat("##X", &values.x, speed))
				changed = true;

			ImGui::PopItemWidth();
			ImGui::SameLine();
		}

		// ================================
		// Y
		{
			PushScopedColorStyle buttonStyle({
				{ ColorStyle::Button,          { 0.1f, 0.6f, 0.1f, 1.0f } },
				{ ColorStyle::ButtonHovered,   { 0.3f, 0.8f, 0.3f, 1.0f } },
				{ ColorStyle::ButtonActive,    { 0.1f, 0.8f, 0.1f, 1.0f } }
			});

			if (ImGui::Button("Y", buttonSize))
			{
				values.y = resetValue;
				changed = true;
			}
			ImGui::SameLine();
			if (ImGui::DragFloat("##Y", &values.y, speed))
				changed = true;
			ImGui::PopItemWidth();
			ImGui::SameLine();
		}

		// ================================
		// Z
		{
			PushScopedColorStyle buttonStyle({
				{ ColorStyle::Button,          { 0.1f, 0.1f, 0.8f, 1.0f } },
				{ ColorStyle::ButtonHovered,   { 0.3f, 0.3f, 0.9f, 1.0f } },
				{ ColorStyle::ButtonActive,    { 0.1f, 0.1f, 0.8f, 1.0f } }
			});

			if (ImGui::Button("Z", buttonSize))
			{
				values.z = resetValue;
				changed = true;
			}

			ImGui::SameLine();
			if (ImGui::DragFloat("##Z", &values.z, speed))
				changed = true;
			ImGui::PopItemWidth();
		}

		ImGui::PopStyleVar(1);
		ImGui::Columns(1);

		ImGui::PopID();

		return changed;
	}

	static bool DrawVec2Control(const char *lable, glm::vec2 &values, float speed = 0.025f, float resetValue = 0.0f, float coloumnWidth = defColWidth)
	{
		bool changed = false;

		ImGui::PushID(lable);

		ImGui::Columns(2);
		ImGui::SetColumnWidth(0, coloumnWidth);
		ImGui::Text(lable);
		ImGui::NextColumn();

		ImGui::PushMultiItemsWidths(2, ImGui::CalcItemWidth());
		ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0, 2));

		float lineHeight = GImGui->FontSize + GImGui->Style.FramePadding.y * 2.0f;
		ImVec2 buttonSize = ImVec2(lineHeight + 3.0f, lineHeight);

		// ================================
		// X
		{
			PushScopedColorStyle buttonStyle({
				{ ColorStyle::Button,          { 0.8f, 0.1f, 0.1f, 1.0f } },
				{ ColorStyle::ButtonHovered,   { 0.9f, 0.3f, 0.3f, 1.0f } },
				{ ColorStyle::ButtonActive,    { 0.8f, 0.1f, 0.1f, 1.0f } }
			});

			if (ImGui::Button("X", buttonSize))
			{
				values.x = resetValue;
				changed = true;
			}

			ImGui::SameLine();

			if (ImGui::DragFloat("##X", &values.x, speed))
				changed = true;

			ImGui::PopItemWidth();
			ImGui::SameLine();
		}

		// ================================
		// Y
		{
			PushScopedColorStyle buttonStyle({
				{ ColorStyle::Button,          { 0.1f, 0.6f, 0.1f, 1.0f } },
				{ ColorStyle::ButtonHovered,   { 0.3f, 0.8f, 0.3f, 1.0f } },
				{ ColorStyle::ButtonActive,    { 0.1f, 0.8f, 0.1f, 1.0f } }
			});

			if (ImGui::Button("Y", buttonSize))
			{
				values.y = resetValue;
				changed = true;
			}
			ImGui::SameLine();
			if (ImGui::DragFloat("##Y", &values.y, speed))
				changed = true;
			ImGui::PopItemWidth();
		}

		ImGui::PopStyleVar(1);
		ImGui::Columns(1);

		ImGui::PopID();

		return changed;
	}

	static bool DrawFloatControl(const char *lable, float *value, float speed = 0.025f, float minValue = 0.0f, float maxValue = 1.0f, float resetValue = 0.0f, float coloumnWidth = defColWidth)
	{
		bool changed = false;

		ImGui::PushID(lable);

		ImGui::Columns(2);
		ImGui::SetColumnWidth(0, coloumnWidth);
		ImGui::Text(lable);
		ImGui::NextColumn();

		ImGui::PushMultiItemsWidths(1, ImGui::CalcItemWidth());
		ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0, 2));

		float lineHeight = GImGui->FontSize + GImGui->Style.FramePadding.y * 2.0f;
		ImVec2 buttonSize = ImVec2(lineHeight + 3.0f, lineHeight);

		{
			PushScopedColorStyle buttonStyle({
				{ ColorStyle::Button,          { 0.5f, 0.5f, 0.5f, 1.0f } },
				{ ColorStyle::ButtonHovered,   { 0.9f, 0.9f, 0.9f, 1.0f } },
				{ ColorStyle::ButtonActive,    { 0.5f, 0.5f, 0.5f, 1.0f } }
			});
			if (ImGui::Button("V", buttonSize))
			{
				*value = resetValue;
				changed = true;
			}
			ImGui::SameLine();
			if (ImGui::DragFloat("##V", value, speed, minValue, maxValue))
				changed = true;
			ImGui::PopItemWidth();
		}

		ImGui::PopStyleVar(1);
		ImGui::Columns(1);

		ImGui::PopID();

		return changed;
	}

	static bool DrawIntControl(const char *lable, int *value, float speed = 1.0f, int minValue = 0, int maxValue = INT_MAX, int resetValue = 0, float coloumnWidth = defColWidth)
	{
		bool changed = false;

		ImGui::PushID(lable);

		ImGui::Columns(2);
		ImGui::SetColumnWidth(0, coloumnWidth);
		ImGui::Text(lable);
		ImGui::NextColumn();

		ImGui::PushMultiItemsWidths(1, ImGui::CalcItemWidth());
		ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0, 2));

		float lineHeight = GImGui->FontSize + GImGui->Style.FramePadding.y * 2.0f;
		ImVec2 buttonSize = ImVec2(lineHeight + 3.0f, lineHeight);

		{
			PushScopedColorStyle buttonStyle({
				{ ColorStyle::Button,          { 0.5f, 0.5f, 0.5f, 1.0f } },
				{ ColorStyle::ButtonHovered,   { 0.9f, 0.9f, 0.9f, 1.0f } },
				{ ColorStyle::ButtonActive,    { 0.5f, 0.5f, 0.5f, 1.0f } }
			});

			if (ImGui::Button("V", buttonSize))
			{
				*value = resetValue;
				changed = true;
			}

			ImGui::SameLine();
			if (ImGui::DragInt("##V", value, speed, minValue, maxValue))
				changed = true;
			ImGui::PopItemWidth();
		}

		ImGui::PopStyleVar(1);
		ImGui::Columns(1);

		ImGui::PopID();

		return changed;
	}
}