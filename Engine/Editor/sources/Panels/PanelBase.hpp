// Copyright (c) Evangelion Manuhutu | ORigin Engine

#ifndef PANEL_BASE_HPP
#define PANEL_BASE_HPP

#include "Origin/Core/Event.h"

#ifndef IMGUI_DEFINE_MATH_OPERATORS
#define IMGUI_DEFINE_MATH_OPERATORS
#endif
#include <imgui.h>
#include <string>
#include <glm/glm.hpp>

#include "Origin/Math/Math.hpp"

namespace origin {

class PanelBase
{
public:
    virtual ~PanelBase() {}

    virtual void Render() = 0;
    virtual void OnUpdate(f32 delta_time) = 0;

    virtual void OnEvent(Event &e) = 0;

    virtual void Close() { m_is_open = false; }
    virtual void Open() { m_is_open = true; }

    void *data = nullptr;
protected:
    glm::vec2 m_viewport_mouse = { 0.0f, 0.0f };
    Rect m_viewport_rect = Rect();
    std::string m_window_title;
    bool m_is_open = true;
};

}

#endif