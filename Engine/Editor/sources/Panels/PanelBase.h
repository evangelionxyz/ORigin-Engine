// Copyright (c) Evangelion Manuhutu | ORigin Engine

#ifndef PANEL_BASE_H
#define PANEL_BASE_H

#include "Origin/Core/Event.h"

#ifndef IMGUI_DEFINE_MATH_OPERATORS
#define IMGUI_DEFINE_MATH_OPERATORS
#endif

#include <imgui.h>
#include <string>

namespace origin {

class PanelBase
{
public:
    PanelBase() {}
    ~PanelBase() {}

    virtual void Render() = 0;
    virtual void OnEvent(Event &e) = 0;

    void Close() { m_Open = false; }
    void Open() { m_Open = true; }

protected:
    std::string m_Title;
    bool m_Open = true;
};

}

#endif