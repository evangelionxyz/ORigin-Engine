﻿// Copyright (c) 2022-present Evangelion Manuhutu | ORigin Engine

#ifndef FMOD_SOUND_H
#define FMOD_SOUND_H

#include <fmod.hpp>
#include <fmod_dsp.h>

namespace origin {
struct FmodDsp;

struct FmodSound
{
    FmodSound() = default;
    FmodSound(std::string name);
    
    void Play();
    void Stop() const;
    void Pause() const;
    void Resume() const;

    void SetPan(float pan) const;
    void SetVolume(float volume) const;
    void SetPitch(float pitch) const;
    void SetMode(FMOD_MODE mode) const;
    void SetFadeIn(u32 fade_in_start_ms, u32 fade_in_end_ms);
    void SetFadeOut(u32 fade_out_start_ms, u32 fade_out_end_ms);
    void AddToChannelGroup(FMOD::ChannelGroup *channel_group);

    void Release();

    [[nodiscard]] float GetPitch() const;
    [[nodiscard]] float GetVolume() const;

    void Update(float delta_time) const;
    void AddDsp(FMOD::DSP* dsp);

    [[no_discard]] FMOD::Sound* GetFmodSound() const;
    [[no_discard]] FMOD::Channel* GetFmodChannel() const;
    [[no_discard]] const std::string &GetName() const;
    [[no_discard]] bool IsPlaying() const;
    [[no_discard]] u32 GetLengthMs() const;
    [[no_discard]] u32 GetPositionMs() const;
    [[no_discard]] FMOD::ChannelGroup *GetChannelGroup() const;

    static Ref<FmodSound> Create(const std::string &name, const std::string &filepath, FMOD_MODE mode = FMOD_DEFAULT | FMOD_LOOP_OFF);
    static Ref<FmodSound> CreateStream(const std::string &name, const std::string &filepath, FMOD_MODE mode = FMOD_DEFAULT | FMOD_LOOP_OFF);
    
private:
    void UpdateFading() const;
    
    FMOD::Sound *m_Sound;
    FMOD::Channel *m_Channel;
    std::string m_Name;

    u32 m_FadeInStartMs;
    u32 m_FadeInEndMs;

    u32 m_FadeOutStartMs;
    u32 m_FadeOutEndMs;

    FMOD::ChannelGroup *m_ChannelGroup;
    std::vector<FMOD::DSP *> m_DSPs;
};

}

#endif