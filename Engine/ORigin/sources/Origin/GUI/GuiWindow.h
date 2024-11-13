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
    GuiWindow(const std::string &title, UI::EWindowFlags flags = UI::EWindowFlags::None);
    GuiWindow(const std::string &title);

    ~GuiWindow();

    void Open();
    void Begin();
    void End();

    void AddColorStyle(std::initializer_list<std::pair<UI::EColorStyle, glm::vec4>> colorStyles);
    void AddStyle(std::initializer_list<std::pair<UI::EStyle, StyleVariant>> styles);

    bool IsOpen() { return m_Open; }

private:
    std::string m_Title;
    bool m_Open;
    bool m_Closing;

    u8 m_ColorStyleCount = 0;
    u8 m_StyleCount = 0;
    UI::EWindowFlags m_WindowFlags;
};

}

#endif
