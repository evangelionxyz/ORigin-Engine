// Copyright (c) 2022-present Evangelion Manuhutu | ORigin Engine
#include "pch.h"
#include "GuiWindow.h"

namespace origin {

GuiWindow::GuiWindow(const std::string &title)
    : m_Title(title), m_WindowFlags(UI::EWindowFlags::None), m_Open(true), m_Closing(false)
{
}

GuiWindow::GuiWindow(const std::string &title, UI::EWindowFlags flags)
    : m_Title(title), m_WindowFlags(flags), m_Open(true), m_Closing(false)
{
}

GuiWindow::~GuiWindow()
{
}

void GuiWindow::Open()
{
    m_Open = true;
}

void GuiWindow::Begin()
{
    if (m_Open)
    {
        m_Closing = false;
        ImGui::Begin(m_Title.c_str(), &m_Open, static_cast<ImGuiWindowFlags>(m_WindowFlags));

        if (!m_Open)
        {
            m_Closing = true;
        }
    }
}

void GuiWindow::End()
{
    if (m_Open || (!m_Open && m_Closing))
    {
        ImGui::End();
    }
    ImGui::PopStyleVar(static_cast<int>(m_StyleCount));
    ImGui::PopStyleColor(static_cast<int>(m_ColorStyleCount));
}

void GuiWindow::AddColorStyle(std::initializer_list<std::pair<UI::EColorStyle, glm::vec4>> colorStyles)
{
    m_ColorStyleCount = static_cast<u8>(colorStyles.size());

    for (u8 i = 0; i < m_ColorStyleCount; ++i)
    {
        auto it = colorStyles.begin() + i;
        ImGui::PushStyleColor(static_cast<ImGuiStyleVar>(it->first), { it->second.x, it->second.y, it->second.z, it->second.w });
    }
}

void GuiWindow::AddStyle(std::initializer_list<std::pair<UI::EStyle, StyleVariant>> styles)
{
    m_StyleCount = static_cast<u8>(styles.size());

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