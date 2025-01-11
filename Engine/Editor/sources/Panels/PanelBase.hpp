// Copyright (c) Evangelion Manuhutu | ORigin Engine

#ifndef PANEL_BASE_H
#define PANEL_BASE_H

#include "Origin/Core/Event.h"

#ifndef IMGUI_DEFINE_MATH_OPERATORS
#define IMGUI_DEFINE_MATH_OPERATORS
#endif

#include <imgui.h>
#include <string>

#include <glm/glm.hpp>

#include "Origin/Math/Math.h"

namespace origin {

class PanelBase
{
public:
    virtual ~PanelBase() {}

    virtual void Render() = 0;
    virtual void OnUpdate(float delta_time) = 0;

    virtual void OnEvent(Event &e) = 0;

    virtual void Close() { m_Open = false; }
    virtual void Open() { m_Open = true; }

protected:
    glm::vec2 m_ViewportMouse = { 0.0f, 0.0f };
    Rect m_ViewportRect = Rect();

    std::string m_Title;
    bool m_Open = true;
};

}

#endif