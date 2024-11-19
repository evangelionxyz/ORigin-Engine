// Copyright (c) Evangelion Manuhutu | ORigin Engine

#include "AudioSystemPanel.h"

namespace origin {

AudioSystemPanel::AudioSystemPanel()
{
}

AudioSystemPanel::~AudioSystemPanel()
{
}

void AudioSystemPanel::Render()
{
    if (m_Open)
    {
        ImGui::Begin("Audio System", &m_Open);

        ImGui::Text("Audio System Page");

        ImGui::End();
    }
}

void AudioSystemPanel::OnEvent(Event &e)
{
    EventDispatcher dispatcher(e);
    dispatcher.Dispatch<KeyPressedEvent>(OGN_BIND_EVENT_FN(AudioSystemPanel::OnKeyPressed));
    dispatcher.Dispatch<MouseButtonPressedEvent>(OGN_BIND_EVENT_FN(AudioSystemPanel::OnMouseButtonPressed));
}

bool AudioSystemPanel::OnKeyPressed(KeyPressedEvent &e)
{
    return false;
}

bool AudioSystemPanel::OnMouseButtonPressed(MouseButtonPressedEvent &e)
{
    return false;
}

}