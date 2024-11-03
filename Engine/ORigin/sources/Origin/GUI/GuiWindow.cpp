// Copyright (c) 2022-present Evangelion Manuhutu | ORigin Engine
#include "pch.h"
#include "GuiWindow.h"

namespace origin {

GuiWindow::GuiWindow(const std::string &title)
    : m_Title(title)
{
}

GuiWindow::GuiWindow(const std::string &title, UI::EWindowFlags flags)
    : m_Title(title), m_WindowFlags(flags)
{
}

GuiWindow::~GuiWindow()
{
    OGN_CORE_ASSERT(m_Showing, "[Gui Window] You forget to call Show()");
    if (m_Showing)
    {
        ImGui::End();
        m_Showing = false;
    }

    ImGui::PopStyleVar(static_cast<int>(m_StyleCount));
    ImGui::PopStyleColor(static_cast<int>(m_ColorStyleCount));
}

void GuiWindow::Show()
{
    if (m_Showing) return;

    ImGui::Begin(m_Title.c_str(), &m_Showing, static_cast<ImGuiWindowFlags>(m_WindowFlags));
    m_Showing = true;
}

void GuiWindow::Hide()
{
}

void GuiWindow::AddColorStyle(std::initializer_list<std::pair<UI::EColorStyle, glm::vec4>> colorStyles)
{
    OGN_CORE_ASSERT(!m_Showing, "[Gui Window] Call Show() after add styles");
    m_ColorStyleCount = colorStyles.size();

    for (int i = 0; i < static_cast<int>(colorStyles.size()); ++i)
    {
        auto it = colorStyles.begin() + i;
        ImGui::PushStyleColor(static_cast<ImGuiStyleVar>(it->first), {it->second.x, it->second.y, it->second.z, it->second.w });
    }
}

void GuiWindow::AddStyle(std::initializer_list<std::pair<UI::EStyle, StyleVariant>> styles)
{
    OGN_CORE_ASSERT(!m_Showing, "[Gui Window] Call Show() after add styles");

    m_StyleCount = styles.size();

    for (const auto &style : styles)
    {
        std::visit([&](auto &&value)
        {
            using T = std::decay_t<decltype(value)>;
            if constexpr (std::is_same_v<T, float>)
                ImGui::PushStyleVar(static_cast<ImGuiStyleVar>(style.first), value);
            else if constexpr (std::is_same_v<T, glm::vec2>)
                ImGui::PushStyleVar(static_cast<ImGuiStyleVar>(style.first), { value.x, value.y });
        }, style.second);
    }
}

}