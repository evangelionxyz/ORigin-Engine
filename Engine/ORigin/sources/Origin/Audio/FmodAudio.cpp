// Copyright (c) 2022-present Evangelion Manuhutu | ORigin Engine

#include "pch.h"
#include "FmodAudio.h"

#include "FmodSound.h"
#include "FmodDsp.h"

#include <ranges>

namespace origin {

static FMOD_RESULT fmod_result;
static FmodAudio *s_FMODAudio = nullptr;

FmodAudio::FmodAudio()
{
    s_FMODAudio = this;

    fmod_result = FMOD::System_Create(&m_System);
    FMOD_CHECK(fmod_result)

    fmod_result = m_System->init(32, FMOD_INIT_NORMAL, nullptr);
    FMOD_CHECK(fmod_result)

    fmod_result = m_System->createChannelGroup("master", &m_MasterGroup);
    FMOD_CHECK(fmod_result)

    // Initialize listener position
    listenerPos = {0.0f, 0.0f, 0.0f};
    listenerVel = {0.0f, 0.0f, 0.0f};
    listenerForward = {0.0f, 0.0f, 1.0f};
    listenerUp = {0.0f, 1.0f, 0.0f};
}

FmodAudio::~FmodAudio()
{
    m_SoundMap.clear();
    fmod_result = m_System->close();
    FMOD_CHECK(fmod_result)
    fmod_result = m_System->release();
    FMOD_CHECK(fmod_result)
}

Ref<FmodSound> FmodAudio::CreateSound(const std::string &name, const std::string &filepath, FMOD_MODE mode)
{
    FMOD::Sound *sound = nullptr;
    m_System->createSound(filepath.c_str(), mode, nullptr, &sound);
    Ref<FmodSound> fmod_sound = CreateRef<FmodSound>(sound, name);
    m_SoundMap[name] = fmod_sound;
    return fmod_sound;
}

Ref<FmodSound> FmodAudio::CreateStream(const std::string &name, const std::string &filepath, FMOD_MODE mode)
{
    FMOD::Sound *stream = nullptr;
    m_System->createStream(filepath.c_str(), mode, nullptr, &stream);
    Ref<FmodSound> fmod_sound = CreateRef<FmodSound>(stream, name);
    m_SoundMap[name] = fmod_sound;
    return fmod_sound;
}

void FmodAudio::SetMasterVolume(float volume) const
{
    FMOD_CHECK(m_MasterGroup->setVolume(volume))
}

void FmodAudio::MuteMaster(bool mute) const
{
    FMOD_CHECK(m_MasterGroup->setMute(mute))
}

void FmodAudio::Update() const
{
    m_System->update();
}

FmodAudio &FmodAudio::GetInstance()
{
    return *s_FMODAudio;
}

FMOD::System* FmodAudio::GetFmodSystem()
{
    return s_FMODAudio->m_System;
}
    
}
