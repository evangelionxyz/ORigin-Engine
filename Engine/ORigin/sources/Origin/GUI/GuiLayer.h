// Copyright (c) 2022-present Evangelion Manuhutu | ORigin Engine

#ifndef GUI_LAYER_H
#define GUI_LAYER_H

#include "Origin/Core/Layer.h"
#include "Origin/Core/AppEvent.h"
#include "Origin/Core/KeyEvent.h"
#include "Origin/Core/MouseEvent.h"

#include <vulkan/vulkan.h>

namespace origin
{
    class SDLWindow;
    class GuiLayer : public Layer
    {
    public:
        GuiLayer(const Ref<SDLWindow> &window);

        void Init();
        void OnAttach() override;
        void OnDetach() override;
        void OnEvent(Event& e) override;
        void BlockEvents(bool block) { m_BlockEvents = block; }
        void Begin();
        void End();

        u32 GetActiveWidgetID();

    private:
        Ref<SDLWindow> m_WindowContext;
        bool m_BlockEvents = false;
        float m_Time = 0.0f;
    };
}

#endif