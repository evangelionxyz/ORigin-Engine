﻿// Copyright (c) Evangelion Manuhutu | ORigin Engine

#include "AudioSystemPanel.hpp"
#include <imgui_internal.h>

namespace origin {

static AudioSystemPanel *s_instance = nullptr;

AudioSystemPanel::AudioSystemPanel()
{
    s_instance = this;
}

AudioSystemPanel::~AudioSystemPanel()
{
}

void AudioSystemPanel::Render()
{
    if (m_is_open)
    {
        ImGui::Begin("Audio System", &m_is_open);

        const ImVec2 canvas_pos = ImGui::GetCursorScreenPos();
        const ImVec2 canvas_size = ImVec2(ImGui::GetContentRegionAvail());

        m_viewport_rect.min = { canvas_pos.x, canvas_pos.y };
        m_viewport_rect.max = { canvas_pos.x + canvas_size.x, canvas_pos.y + canvas_size.y };

        ImDrawList *dl = ImGui::GetWindowDrawList();

        // Margins and dimensions
        Margin channel_margin(20.0f, 20.0f, 20.0f, 30.0f);
        Margin gain_margin(30.0f, 50.0f);
        Margin label_margin(10.0f);
        f32 channel_width = 100.0f;

        static f32 horizontal_scroll = 0.0f;
        static bool is_scrolling = false;
        static ImVec2 scroll_start_mouse_pos;
        static f32 scroll_start_offset = 0.0f;

        // master channel
        Rect master_channel_canvas = m_viewport_rect - Rect({ horizontal_scroll, 0.0f }, { 0.0f, 0.0f });
        f32 master_volume = FmodAudio::GetMasterVolume();
        RenderAudioChannel(master_volume, dl, master_channel_canvas, 0, channel_width,
            channel_margin, gain_margin, label_margin, "Master", "Master\nChannel");
        FmodAudio::SetMasterVolume(master_volume);

        // other audio channel
        Rect audio_channel_canvas = m_viewport_rect + Rect({ horizontal_scroll, 0.0f }, {0.0f, 0.0f});
        for (size_t x = 0; x < m_Sounds.size(); ++x)
        {
            f32 volume = m_Sounds[x]->GetVolume();
            std::string bottom_label = "Channel " + std::to_string(x + 1);
            RenderAudioChannel(volume, dl, audio_channel_canvas, static_cast<i32>(x + 1), channel_width,
                channel_margin, gain_margin, label_margin, m_Sounds[0]->GetName(), bottom_label);
            m_Sounds[0]->SetVolume(volume);
        }

        // horizontal slider
        // + 1 (master channel)
        const f32 total_channel_width = static_cast<f32>(m_Sounds.size() + 1) * channel_width + (channel_margin.right * 2.0f);
        const f32 visible_width = m_viewport_rect.max.x - m_viewport_rect.min.x;

        if (total_channel_width > visible_width)
        {
            constexpr f32 scrollbar_height = 15.0f;
            const f32 scrollbar_x = m_viewport_rect.min.x + channel_margin.left;
            const f32 scrollbar_y = m_viewport_rect.max.y - scrollbar_height - 5.0f;
            const f32 scrollbar_width = visible_width - (channel_margin.right * 2.0f);
            const f32 scrollbar_thumb_width = (visible_width / total_channel_width) * scrollbar_width;
            const f32 scrollbar_thumb_x = scrollbar_x + (horizontal_scroll / (total_channel_width - visible_width)) * (scrollbar_width - scrollbar_thumb_width);

            Rect scrollbar_rect = Rect(
                { scrollbar_thumb_x, scrollbar_y },
                { scrollbar_thumb_x + scrollbar_thumb_width, scrollbar_y + scrollbar_height }
            );

            // draw scrollbar thumb
            dl->AddRectFilled(
                { scrollbar_rect.min.x, scrollbar_rect.min.y },
                { scrollbar_rect.max.x, scrollbar_rect.max.y },
                ImColor{ 0.5f, 0.5f, 0.5f, 1.0f }
            );

            const ImVec2 &mouse_pos = ImGui::GetMousePos();

            if (scrollbar_rect.Contains(mouse_pos) && ImGui::IsMouseClicked(0))
            {
                is_scrolling = true;
                scroll_start_mouse_pos = mouse_pos;
                scroll_start_offset = horizontal_scroll;
            }

            if (is_scrolling)
            {
                if (ImGui::IsMouseReleased(0))
                    is_scrolling = false;
                else
                {
                    float delta = mouse_pos.x - scroll_start_mouse_pos.x;
                    horizontal_scroll = scroll_start_offset + (delta / (scrollbar_width - scrollbar_thumb_width))
                        * (total_channel_width - visible_width);
                    horizontal_scroll = std::clamp(horizontal_scroll, 0.0f, total_channel_width - visible_width);
                }
            }
        }

        ImGui::End();
    }
}

void AudioSystemPanel::OnUpdate(float delta_time)
{
}

void AudioSystemPanel::OnEvent(Event &e)
{
    EventDispatcher dispatcher(e);
    dispatcher.Dispatch<KeyPressedEvent>(OGN_BIND_EVENT_FN(AudioSystemPanel::OnKeyPressed));
    dispatcher.Dispatch<MouseButtonPressedEvent>(OGN_BIND_EVENT_FN(AudioSystemPanel::OnMouseButtonPressed));
}

AudioSystemPanel* AudioSystemPanel::GetInstance()
{
    return s_instance;
}

void AudioSystemPanel::RenderAudioChannel(float &volume, ImDrawList *dl, const Rect &m_ViewportRect, i32 channel_index,
                                          float channel_width, const Margin &channel_margin, const Margin &gain_margin,
                                          const Margin &label_margin, const std::string &top_label, const std::string &bottom_label)
{
    const Rect channel_size = Rect
    ({
        m_ViewportRect.min.x + channel_margin.left + (static_cast<f32>(channel_index) * channel_width),
        m_ViewportRect.min.y + channel_margin.top
        },
    {
        m_ViewportRect.min.x + channel_margin.left + (static_cast<f32>(channel_index + 1) * channel_width) - channel_margin.right,
        m_ViewportRect.max.y - channel_margin.bottom
    });

    const Rect gain_size = Rect
    ({
        channel_size.min.x + gain_margin.left,
        channel_size.min.y + gain_margin.top
    },
    {
        channel_size.max.x -  gain_margin.right,
        channel_size.max.y - gain_margin.bottom
    });

    // draw channel background
    dl->AddRectFilled(
        { channel_size.min.x, channel_size.min.y },
        { channel_size.max.x, channel_size.max.y },
        ImColor{ 0.1f, 0.1f, 0.1f, 1.0f });

    // draw gain background
    dl->AddRectFilled(
        { gain_size.min.x, gain_size.min.y },
        { gain_size.max.x, gain_size.max.y },
        ImColor{ 0.6f, 0.6f, 0.6f, 1.0f }
        );

    // draw gain foreground
    float gain_foreground_height = (1.0f - volume) * (gain_size.max.y - gain_size.min.y);
    dl->AddRectFilled(
        { gain_size.min.x, gain_size.min.y + gain_foreground_height },
        { gain_size.max.x, gain_size.max.y },
        ImColor{ 0.1f, 0.3f, 0.8f, 1.0f });

    // draw interactive area of gain vertical slider
    Rect volume_slider_size = Rect(
        { gain_size.min.x - 5.0f, gain_size.min.y + gain_foreground_height - 10.0f },
        { gain_size.max.x + 5.0f, gain_size.min.y + gain_foreground_height + 10.0f }
    );

    dl->AddRectFilled(
        { volume_slider_size.min.x, volume_slider_size.min.y },
        { volume_slider_size.max.x, volume_slider_size.max.y },
        ImColor{ 0.8f, 0.2f, 0.2f, 1.0f }
    );

    static bool mouse_dragging = false;

    const ImVec2 &mouse_pos = ImGui::GetMousePos();
    if (volume_slider_size.Contains(mouse_pos) && ImGui::IsMouseClicked(0))
    {
        if (m_CurrentDragChannel == -1)
        {
            m_CurrentDragChannel = channel_index;
            mouse_dragging = true;
        }
    }

    if (mouse_dragging && m_CurrentDragChannel == channel_index)
    {
        if (ImGui::IsMouseReleased(0))
        {
            m_CurrentDragChannel = -1;
            mouse_dragging = false;
        }

        float knob_height = gain_size.max.y - gain_size.min.y;
        volume = 1.0f - (mouse_pos.y - gain_size.min.y) / knob_height;
        volume = std::clamp(volume, 0.f, 1.0f);
    }

    // draw top label
    dl->AddText(
        { channel_size.min.x + label_margin.left, channel_size.min.y + label_margin.top },
        ImColor{ 1.0f, 1.0f, 1.0f, 1.0f }, top_label.c_str());


    // channel label
    dl->AddText(
        { channel_size.min.x + label_margin.left, gain_size.max.y + label_margin.bottom },
        ImColor{ 0.8f, 0.8f, 0.8f, 1.0f }, bottom_label.c_str());
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