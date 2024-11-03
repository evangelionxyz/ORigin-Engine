// Copyright (c) 2022-present Evangelion Manuhutu | ORigin Engine

#ifndef GUI_WINDOW_H
#define GUI_WINDOW_H

#include "UI.h"

#include <string>
#include <variant>

namespace origin {

using StyleVariant = std::variant<float, glm::vec2>;

class GuiWindow
{
public:
    GuiWindow() = default;

    GuiWindow(const std::string &title, UI::EWindowFlags flags);
    GuiWindow(const std::string &title);

    ~GuiWindow();

    void Show();
    void Hide();

    void AddColorStyle(std::initializer_list<std::pair<UI::EColorStyle, glm::vec4>> colorStyles);
    void AddStyle(std::initializer_list<std::pair<UI::EStyle, StyleVariant>> styles);

    bool IsShowing() { return m_Showing; }

private:
    std::string m_Title;
    bool m_Showing = false;

    u8 m_ColorStyleCount = 0;
    u8 m_StyleCount = 0;
    UI::EWindowFlags m_WindowFlags = UI::EWindowFlags::None;
};

}

#endif
