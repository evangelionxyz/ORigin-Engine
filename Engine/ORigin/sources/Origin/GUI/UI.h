// Copyright (c) 2022-present Evangelion Manuhutu | ORigin Engine

#ifndef UI_H
#define UI_H

#define IMGUI_DEFINE_MATH_OPERATORS
#include <imgui.h>
#include <imgui_internal.h>

#include <glm/glm.hpp>
#include <functional>

namespace origin::UI {

enum class EWindowFlags
{
    None = 0,
    NoTitleBar = 1 << 0,   // Disable title-bar
    NoResize = 1 << 1,   // Disable user resizing with the lower-right grip
    NoMove = 1 << 2,   // Disable user moving the window
    NoScrollbar = 1 << 3,   // Disable scrollbars (window can still scroll with mouse or programmatically)
    NoScrollWithMouse = 1 << 4,   // Disable user vertically scrolling with mouse wheel. On child window, mouse wheel will be forwarded to the parent unless NoScrollbar is also set.
    NoCollapse = 1 << 5,   // Disable user collapsing window by double-clicking on it. Also referred to as Window Menu Button (e.g. within a docking node).
    AlwaysAutoResize = 1 << 6,   // Resize every window to its content every frame
    NoBackground = 1 << 7,   // Disable drawing background color (WindowBg, etc.) and outside border. Similar as using SetNextWindowBgAlpha(0.0f).
    NoSavedSettings = 1 << 8,   // Never load/save settings in .ini file
    NoMouseInputs = 1 << 9,   // Disable catching mouse, hovering test with pass through.
    MenuBar = 1 << 10,  // Has a menu-bar
    HorizontalScrollbar = 1 << 11,  // Allow horizontal scrollbar to appear (off by default). You may use SetNextWindowContentSize(ImVec2(width,0.0f)); prior to calling Begin() to specify width. Read code in imgui_demo in the "Horizontal Scrolling" section.
    NoFocusOnAppearing = 1 << 12,  // Disable taking focus when transitioning from hidden to visible state
    NoBringToFrontOnFocus = 1 << 13,  // Disable bringing window to front when taking focus (e.g. clicking on it or programmatically giving it focus)
    AlwaysVerticalScrollbar = 1 << 14,  // Always show vertical scrollbar (even if ContentSize.y < Size.y)
    AlwaysHorizontalScrollbar = 1 << 15,  // Always show horizontal scrollbar (even if ContentSize.x < Size.x)
    NoNavInputs = 1 << 16,  // No gamepad/keyboard navigation within the window
    NoNavFocus = 1 << 17,  // No focusing toward this window with gamepad/keyboard navigation (e.g. skipped by CTRL+TAB)
    UnsavedDocument = 1 << 18,  // Display a dot next to the title. When used in a tab/docking context, tab is selected when clicking the X + closure is not assumed (will wait for user to stop submitting the tab). Otherwise closure is assumed when pressing the X, so if you keep submitting the tab may reappear at end of tab bar.
    NoDocking = 1 << 19,  // Disable docking of this window
    NoNav = NoNavInputs | NoNavFocus,
    NoDecoration = NoTitleBar | NoResize | NoScrollbar | NoCollapse,
    NoInputs = NoMouseInputs | NoNavInputs | NoNavFocus,

    // [Internal]
    ChildWindow = 1 << 24,  // Don't use! For internal use by BeginChild()
    Tooltip = 1 << 25,  // Don't use! For internal use by BeginTooltip()
    Popup = 1 << 26,  // Don't use! For internal use by BeginPopup()
    Modal = 1 << 27,  // Don't use! For internal use by BeginPopupModal()
    ChildMenu = 1 << 28,  // Don't use! For internal use by BeginMenu()
    DockNodeHost = 1 << 29,  // Don't use! For internal use by Begin()/NewFrame()

    // Obsolete names
#ifndef IMGUI_DISABLE_OBSOLETE_FUNCTIONS
    AlwaysUseWindowPadding = 1 << 30,  // Obsoleted in 1.90.0: Use ImGuiChildFlags_AlwaysUseWindowPadding in BeginChild() call.
    NavFlattened = 1 << 31,  // Obsoleted in 1.90.9: Use ImGuiChildFlags_NavFlattened in BeginChild() call.
#endif
};


enum class EStyle
{
	// Enum name -------------------------- // Member in ImGuiStyle structure (see ImGuiStyle for descriptions)
	Alpha,                    // float     Alpha
	DisabledAlpha,            // float     DisabledAlpha
	WindowPadding,            // ImVec2    WindowPadding
	WindowRounding,           // float     WindowRounding
	WindowBorderSize,         // float     WindowBorderSize
	WindowMinSize,            // ImVec2    WindowMinSize
	WindowTitleAlign,         // ImVec2    WindowTitleAlign
	ChildRounding,            // float     ChildRounding
	ChildBorderSize,          // float     ChildBorderSize
	PopupRounding,            // float     PopupRounding
	PopupBorderSize,          // float     PopupBorderSize
	FramePadding,             // ImVec2    FramePadding
	FrameRounding,            // float     FrameRounding
	FrameBorderSize,          // float     FrameBorderSize
	ItemSpacing,              // ImVec2    ItemSpacing
	ItemInnerSpacing,         // ImVec2    ItemInnerSpacing
	IndentSpacing,            // float     IndentSpacing
	CellPadding,              // ImVec2    CellPadding
	ScrollbarSize,            // float     ScrollbarSize
	ScrollbarRounding,        // float     ScrollbarRounding
	GrabMinSize,              // float     GrabMinSize
	GrabRounding,             // float     GrabRounding
	TabRounding,              // float     TabRounding
	TabBorderSize,            // float     TabBorderSize
	TabBarBorderSize,         // float     TabBarBorderSize
	TabBarOverlineSize,       // float     TabBarOverlineSize
	TableAngledHeadersAngle,  // float     TableAngledHeadersAngle
	TableAngledHeadersTextAlign,// ImVec2  TableAngledHeadersTextAlign
	ButtonTextAlign,          // ImVec2    ButtonTextAlign
	SelectableTextAlign,      // ImVec2    SelectableTextAlign
	SeparatorTextBorderSize,  // float     SeparatorTextBorderSize
	SeparatorTextAlign,       // ImVec2    SeparatorTextAlign
	SeparatorTextPadding,     // ImVec2    SeparatorTextPadding
	DockingSeparatorSize,     // float     DockingSeparatorSize
};

// Enumeration for PushStyleColor() / PopStyleColor()
enum class EColorStyle
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
};

struct StyleVarBase
{
};

class ScopedColorStyle
{
public:
	ScopedColorStyle(std::initializer_list<std::pair<EColorStyle, glm::vec4>> styles)
		: m_Styles(styles)
	{
		for (auto s : m_Styles)
            ImGui::PushStyleColor(static_cast<ImGuiCol>(s.first), ImVec4(s.second.x, s.second.y, s.second.z, s.second.w));
	}
	~ScopedColorStyle()
	{
		ImGui::PopStyleColor(static_cast<int>(m_Styles.size()));
	}

private:
	std::vector<std::pair<EColorStyle, glm::vec4>> m_Styles;
};

static float defColWidth = 100.0f;

static bool DrawButton(const char *text, bool *value = nullptr)
{
	bool ret = false;

	ImGui::PushID((void *)&text);

	float lineHeight = GImGui->FontSize + GImGui->Style.FramePadding.y * 2.0f;
	float lineWidth = GImGui->FontSize + GImGui->Style.FramePadding.x * (ImGui::CalcTextSize(text).x / 4.0f);
	ImVec2 btSize = ImVec2(lineWidth, lineHeight);

	ret = ImGui::Button(text, btSize);
	if (value)
		*value = ret;

	ImGui::PopID();
	return ret;
}

static bool DrawButtonWithColumn(const char *label, const char *text, bool *value = nullptr, std::function<void()> func = std::function<void()>(), float coloumnWidth = defColWidth)
{
	bool ret = false;

	ImGui::PushID(label);

	float lineHeight = GImGui->FontSize + GImGui->Style.FramePadding.y * 2.0f;
	float lineWidth = GImGui->FontSize + GImGui->Style.FramePadding.x * (ImGui::CalcTextSize(text).x / 4.0f);
	ImVec2 btSize = ImVec2(lineWidth, lineHeight);

	ImGui::BeginColumns(label, 2, ImGuiOldColumnFlags_GrowParentContentsSize);
	ImGui::SetColumnWidth(0, coloumnWidth);
	ImGui::Text("%s", label);
	ImGui::NextColumn();

	ret = ImGui::Button(text, btSize);

	if (func != nullptr)
		func();

	if (value)
		*value = ret;

	ImGui::EndColumns();
	ImGui::PopID();

	return ret;
}

static bool DrawCheckbox(const char *label, bool *value, float coloumnWidth = defColWidth)
{
	bool ret = false;

	ImGui::PushID(label);

	ImGui::BeginColumns(label, 2, ImGuiOldColumnFlags_GrowParentContentsSize);
	ImGui::SetColumnWidth(0, coloumnWidth);
	ImGui::Text("%s", label);
	ImGui::NextColumn();

	ret = ImGui::Checkbox("##check_box", value);
	ImGui::EndColumns();
	ImGui::PopID();

	return ret;
}

static bool DrawCheckbox2(const char *label, bool *x, bool *y, float coloumnWidth = defColWidth)
{
	bool ret = false;

	ImGui::PushID(label);

	ImGui::BeginColumns(label, 3, ImGuiOldColumnFlags_GrowParentContentsSize);
	ImGui::SetColumnWidth(0, coloumnWidth);
	ImGui::Text("%s", label);
	ImGui::NextColumn();
	if (ImGui::Checkbox("X", x))
	{
		ret = true;
	}
	ImGui::NextColumn();
	if (ImGui::Checkbox("Y", y))
	{
		ret = true;
	}
	ImGui::EndColumns();
	ImGui::PopID();

	return ret;
}

static bool DrawCheckbox3(const char *label, bool *x, bool *y, bool *z, float coloumnWidth = defColWidth)
{
	bool ret = false;

	ImGui::PushID(label);

	ImGui::BeginColumns(label, 4, ImGuiOldColumnFlags_GrowParentContentsSize);
	ImGui::SetColumnWidth(0, coloumnWidth);
	ImGui::Text("%s", label);
	ImGui::NextColumn();
	if (ImGui::Checkbox("X", x))
	{
		ret = true;
	}
	ImGui::NextColumn();
	if (ImGui::Checkbox("Y", y))
	{
		ret = true;
	}
	ImGui::NextColumn();
	if (ImGui::Checkbox("Z", z))
	{
		ret = true;
	}
	ImGui::EndColumns();
	ImGui::PopID();

	return ret;
}

static bool DrawVec4Control(const char *label, glm::vec4 &values, float speed = 0.025f, float resetValue = 0.0f, float coloumnWidth = defColWidth)
{
	bool changed = false;
	ImGui::PushID(label);

	ImGui::Columns(2);
	ImGui::SetColumnWidth(0, coloumnWidth);
	ImGui::Text("%s", label);
	ImGui::NextColumn();

	ImGui::PushMultiItemsWidths(4, ImGui::CalcItemWidth());
	ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0, 5));

	float lineHeight = GImGui->FontSize + GImGui->Style.FramePadding.y * 2.0f;
	ImVec2 buttonSize = ImVec2(lineHeight, lineHeight);

	// ================================
	// X
	{
		ScopedColorStyle buttonStyle({
			{ EColorStyle::Button,          { 0.8f, 0.1f, 0.1f, 1.0f } },
			{ EColorStyle::ButtonHovered,   { 0.9f, 0.3f, 0.3f, 1.0f } },
			{ EColorStyle::ButtonActive,    { 0.8f, 0.1f, 0.1f, 1.0f } }
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
		ScopedColorStyle buttonStyle({
			{ EColorStyle::Button,          { 0.1f, 0.6f, 0.1f, 1.0f } },
			{ EColorStyle::ButtonHovered,   { 0.3f, 0.8f, 0.3f, 1.0f } },
			{ EColorStyle::ButtonActive,    { 0.1f, 0.8f, 0.1f, 1.0f } }
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
		ScopedColorStyle buttonStyle({
			{ EColorStyle::Button,          { 0.1f, 0.1f, 0.8f, 1.0f } },
			{ EColorStyle::ButtonHovered,   { 0.3f, 0.3f, 0.9f, 1.0f } },
			{ EColorStyle::ButtonActive,    { 0.1f, 0.1f, 0.8f, 1.0f } }
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
		ScopedColorStyle buttonStyle({
			{ EColorStyle::Button,          { 0.8f, 0.8f, 0.1f, 1.0f } },
			{ EColorStyle::ButtonHovered,   { 0.9f, 0.9f, 0.3f, 1.0f } },
			{ EColorStyle::ButtonActive,    { 0.8f, 0.8f, 0.1f, 1.0f } }
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

static bool DrawVec3Control(const char *label, glm::vec3 &values, float speed = 0.025f, float resetValue = 0.0f, float coloumnWidth = defColWidth)
{
	bool changed = false;

	ImGui::PushID(label);

	ImGui::Columns(2);
	ImGui::SetColumnWidth(0, coloumnWidth);
	ImGui::Text("%s", label);
	ImGui::NextColumn();

	ImGui::PushMultiItemsWidths(3, ImGui::CalcItemWidth());
	ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0, 2));

	float lineHeight = GImGui->FontSize + GImGui->Style.FramePadding.y * 2.0f;
	ImVec2 buttonSize = ImVec2(lineHeight, lineHeight);

	// ================================
	// X
	{
		ScopedColorStyle buttonStyle({
			{ EColorStyle::Button,          { 0.8f, 0.1f, 0.1f, 1.0f } },
			{ EColorStyle::ButtonHovered,   { 0.9f, 0.3f, 0.3f, 1.0f } },
			{ EColorStyle::ButtonActive,    { 0.8f, 0.1f, 0.1f, 1.0f } }
			});

		if (ImGui::Button("X", buttonSize))
		{
			values.x = resetValue;
			changed = true;
		}

		ImGui::SameLine();

		changed |= ImGui::DragFloat("##X", &values.x, speed);


		ImGui::PopItemWidth();
		ImGui::SameLine();
	}

	// ================================
	// Y
	{
		ScopedColorStyle buttonStyle({
			{ EColorStyle::Button,          { 0.1f, 0.6f, 0.1f, 1.0f } },
			{ EColorStyle::ButtonHovered,   { 0.3f, 0.8f, 0.3f, 1.0f } },
			{ EColorStyle::ButtonActive,    { 0.1f, 0.8f, 0.1f, 1.0f } }
			});

		if (ImGui::Button("Y", buttonSize))
		{
			values.y = resetValue;
			changed = true;
		}
		ImGui::SameLine();
		changed |= ImGui::DragFloat("##Y", &values.y, speed);

		ImGui::PopItemWidth();
		ImGui::SameLine();
	}

	// ================================
	// Z
	{
		ScopedColorStyle buttonStyle({
			{ EColorStyle::Button,          { 0.1f, 0.1f, 0.8f, 1.0f } },
			{ EColorStyle::ButtonHovered,   { 0.3f, 0.3f, 0.9f, 1.0f } },
			{ EColorStyle::ButtonActive,    { 0.1f, 0.1f, 0.8f, 1.0f } }
			});

		if (ImGui::Button("Z", buttonSize))
		{
			values.z = resetValue;
			changed = true;
		}

		ImGui::SameLine();
		changed |= ImGui::DragFloat("##Z", &values.z, speed);
		ImGui::PopItemWidth();
	}

	ImGui::PopStyleVar(1);
	ImGui::Columns(1);

	ImGui::PopID();

	return changed;
}

static bool DrawVec2Control(const char *label, glm::vec2 &values, float speed = 0.025f, float resetValue = 0.0f, float coloumnWidth = defColWidth)
{
	bool changed = false;

	ImGui::PushID(label);

	ImGui::Columns(2);
	ImGui::SetColumnWidth(0, coloumnWidth);
	ImGui::Text("%s", label);
	ImGui::NextColumn();

	ImGui::PushMultiItemsWidths(2, ImGui::CalcItemWidth());
	ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0, 2));

	float lineHeight = GImGui->FontSize + GImGui->Style.FramePadding.y * 2.0f;
	ImVec2 buttonSize = ImVec2(lineHeight + 3.0f, lineHeight);

	// ================================
	// X
	{
		ScopedColorStyle buttonStyle({
			{ EColorStyle::Button,          { 0.8f, 0.1f, 0.1f, 1.0f } },
			{ EColorStyle::ButtonHovered,   { 0.9f, 0.3f, 0.3f, 1.0f } },
			{ EColorStyle::ButtonActive,    { 0.8f, 0.1f, 0.1f, 1.0f } }
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
		ScopedColorStyle buttonStyle({
			{ EColorStyle::Button,          { 0.1f, 0.6f, 0.1f, 1.0f } },
			{ EColorStyle::ButtonHovered,   { 0.3f, 0.8f, 0.3f, 1.0f } },
			{ EColorStyle::ButtonActive,    { 0.1f, 0.8f, 0.1f, 1.0f } }
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

static bool DrawFloatControl(const char *label, float *value, float speed = 0.025f, float minValue = 0.0f, float maxValue = 1.0f, float resetValue = 0.0f, float coloumnWidth = defColWidth)
{
	bool changed = false;

	ImGui::PushID(label);

	ImGui::Columns(2);
	ImGui::SetColumnWidth(0, coloumnWidth);
	ImGui::Text("%s", label);
	ImGui::NextColumn();

	ImGui::PushMultiItemsWidths(1, ImGui::CalcItemWidth());
	ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0, 2));

	float lineHeight = GImGui->FontSize + GImGui->Style.FramePadding.y * 2.0f;
	ImVec2 buttonSize = ImVec2(lineHeight + 3.0f, lineHeight);

	{
		ScopedColorStyle buttonStyle({
			{ EColorStyle::Button,          { 0.5f, 0.5f, 0.5f, 1.0f } },
			{ EColorStyle::ButtonHovered,   { 0.9f, 0.9f, 0.9f, 1.0f } },
			{ EColorStyle::ButtonActive,    { 0.5f, 0.5f, 0.5f, 1.0f } }
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

static bool DrawIntControl(const char *label, int *value, float speed = 1.0f, int minValue = 0, int maxValue = INT_MAX, int resetValue = 0, float coloumnWidth = defColWidth)
{
	bool changed = false;

	ImGui::PushID(label);

	ImGui::Columns(2);
	ImGui::SetColumnWidth(0, coloumnWidth);
	ImGui::Text("%s", label);
	ImGui::NextColumn();

	ImGui::PushMultiItemsWidths(1, ImGui::CalcItemWidth());
	ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0, 2));

	float lineHeight = GImGui->FontSize + GImGui->Style.FramePadding.y * 2.0f;
	ImVec2 buttonSize = ImVec2(lineHeight + 3.0f, lineHeight);

	{
		ScopedColorStyle buttonStyle({
			{ EColorStyle::Button,          { 0.5f, 0.5f, 0.5f, 1.0f } },
			{ EColorStyle::ButtonHovered,   { 0.9f, 0.9f, 0.9f, 1.0f } },
			{ EColorStyle::ButtonActive,    { 0.5f, 0.5f, 0.5f, 1.0f } }
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

#endif