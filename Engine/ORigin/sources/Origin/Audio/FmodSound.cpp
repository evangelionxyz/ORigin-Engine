// Copyright (c) 2022-present Evangelion Manuhutu | ORigin Engine

#include "pch.h"
#include "FmodSound.h"
#include "FmodDsp.h"

namespace origin {

FmodSound::FmodSound(FMOD::Sound* sound, std::string name)
    : m_Sound(sound), m_Channel(nullptr), m_Paused(false)
{
}

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
    
}
void FmodSound::Stop()
{
    
}
void FmodSound::Pause()
{
    
}

void FmodSound::Resume()
{
    
}
void FmodSound::AddDsp(Scope<FmodDsp> dsp)
{
    
}
}
