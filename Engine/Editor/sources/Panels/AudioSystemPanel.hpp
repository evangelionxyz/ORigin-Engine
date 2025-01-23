// Copyright (c) Evangelion Manuhutu | ORigin Engine

#ifndef AUDIO_SYSTEM_HPP
#define AUDIO_SYSTEM_HPP

#include "PanelBase.hpp"

#include "Origin/Core/KeyEvent.h"
#include "Origin/Core/MouseEvent.h"

#include "Origin/Audio/FmodSound.h"
#include "Origin/Audio/FmodAudio.h"
#include "Origin/Audio/FmodDsp.h"

namespace origin {

class AudioSystemPanel : public PanelBase
{
public:
    AudioSystemPanel();
    ~AudioSystemPanel();

    void Render() override;
    void OnUpdate(float delta_time) override;

    void OnEvent(Event &e) override;

private:
    void RenderAudioChannel(float &volume, ImDrawList *dl, const Rect &canvas_rect, i32 channel_index,
        float channel_width, const Margin &channel_margin, const Margin &gain_margin,
        const Margin &label_margin, const std::string &top_label, const std::string &bottom_label);

    bool OnKeyPressed(KeyPressedEvent &e);
    bool OnMouseButtonPressed(MouseButtonPressedEvent &e);

    std::vector<Ref<FmodSound>> m_Sounds;
    i32 m_CurrentDragChannel = -1;
};

}

#endif