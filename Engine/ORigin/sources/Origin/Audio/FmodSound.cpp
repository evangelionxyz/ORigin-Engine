// Copyright (c) 2022-present Evangelion Manuhutu | ORigin Engine

#include "pch.h"
#include "FmodSound.h"
#include "FmodDsp.h"

namespace origin {

FmodSound::~FmodSound()
{
    if (m_Sound)
    {
        m_Sound->release();
        m_Sound = nullptr;
    }
}

void FmodSound::Play()
{
    FmodAudio::GetFmodSystem()->playSound(m_Sound, FmodAudio::GetFmodChannelGroup(), m_Paused, &m_Channel);
}

void FmodSound::Stop() const
{
    m_Channel->stop();
}

void FmodSound::Pause()
{
    m_Channel->setPaused(true);
    m_Paused = true;
}

void FmodSound::Resume()
{
    m_Channel->setPaused(false);
    m_Paused = false;
}

void FmodSound::SetPan(const float pan) const
{
    m_Channel->setPan(pan);
}

void FmodSound::SetVolume(float volume) const
{
    volume = std::clamp(volume, 0.0f, 1.0f);
    m_Channel->setVolume(volume);
}

void FmodSound::SetPitch(const float pitch) const
{
    m_Channel->setPitch(pitch);
}

void FmodSound::SetFadeIn(u32 fade_in_start_ms, u32 fade_in_end_ms)
{
    m_FadeInStartMs = fade_in_start_ms;
    m_FadeInEndMs = fade_in_end_ms;
}

void FmodSound::SetFadeOut(u32 fade_out_start_ms, u32 fade_out_end_ms)
{
    m_FadeOutStartMs = fade_out_start_ms;
    m_FadeOutEndMs = fade_out_end_ms;
}

float FmodSound::GetPitch() const
{
    float pitch;
    m_Channel->getPitch(&pitch);
    return pitch;
}

float FmodSound::GetVolume() const
{
    float volume;
    m_Channel->getVolume(&volume);
    return volume;
}

void FmodSound::Update([[maybe_unused]] float delta_time) const
{
    UpdateFading();
}

void FmodSound::AddDsp(const Ref<FmodDsp>& dsp)
{
    m_Dsps.push_back(dsp);
}

FMOD::Sound* FmodSound::GetSound() const
{
    return m_Sound;
}

FMOD::Channel* FmodSound::GetChannel() const
{
    return m_Channel;
}

const std::string& FmodSound::GetName() const
{
    return m_Name;
}

bool FmodSound::IsPlaying() const
{
    bool is_playing = false;
    m_Channel->isPlaying(&is_playing);
    return is_playing;
}

u32 FmodSound::GetLengthMs() const
{
    u32 length;
    m_Sound->getLength(&length, FMOD_TIMEUNIT_MS);
    return length;
}

u32 FmodSound::GetPositionMs() const
{
    u32 position;
    m_Channel->getPosition(&position, FMOD_TIMEUNIT_MS);
    return position;
}

Ref<FmodSound> FmodSound::Create(const std::string &name, const std::string &filepath, FMOD_MODE mode)
{
    Ref<FmodSound> fmod_sound = CreateRef<FmodSound>();
    FmodAudio::GetFmodSystem()->createSound(filepath.c_str(), mode, nullptr, &fmod_sound->m_Sound);
    FmodAudio::InsertFmodSound(name, fmod_sound);
    return fmod_sound;
}

Ref<FmodSound> FmodSound::CreateStream(const std::string& name, const std::string& filepath, FMOD_MODE mode)
{
    Ref<FmodSound> fmod_sound = CreateRef<FmodSound>();
    FmodAudio::GetFmodSystem()->createStream(filepath.c_str(), mode, nullptr, &fmod_sound->m_Sound);
    FmodAudio::InsertFmodSound(name, fmod_sound);
    return fmod_sound;
}

void FmodSound::UpdateFading() const
{
    u32 position = 0;
    
    m_Channel->getPosition(&position, FMOD_TIMEUNIT_MS);
    float volume = GetVolume();
    if (position >= m_FadeInStartMs && position <= m_FadeInEndMs)
    {
        const float fade_progress = static_cast<float>(position - m_FadeInStartMs)
            / static_cast<float>(m_FadeInEndMs - m_FadeInStartMs);
        volume = fade_progress;
    }

    if (position >= m_FadeOutStartMs && position <= m_FadeOutEndMs)
    {
        const float fade_progress = static_cast<float>(position - m_FadeOutStartMs)
            / static_cast<float>(m_FadeOutEndMs - m_FadeOutStartMs);
        volume = 1.0f - fade_progress;
    }

    SetVolume(volume);
}

}
