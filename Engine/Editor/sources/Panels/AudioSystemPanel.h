// Copyright (c) Evangelion Manuhutu | ORigin Engine

#ifndef AUDIO_SYSTEM_H
#define AUDIO_SYSTEM_H

#include "PanelBase.h"

#include "Origin/Core/KeyEvent.h"
#include "Origin/Core/MouseEvent.h"

namespace origin {

class AudioSystemPanel : public PanelBase
{
public:
    AudioSystemPanel();
    ~AudioSystemPanel();

    void Render() override;

    void OnEvent(Event &e) override;

private:
    bool OnKeyPressed(KeyPressedEvent &e);
    bool OnMouseButtonPressed(MouseButtonPressedEvent &e);
};

}

#endif