// Copyright (c) 2022-present Evangelion Manuhutu | ORigin Engine

#ifndef FMOD_SOUND_H
#define FMOD_SOUND_H

#include <fmod.hpp>

namespace origin {
struct FmodDsp;

struct FmodSound
{
    FmodSound(FMOD::Sound *sound, std::string name);
    ~FmodSound();

    void Play();
    void Stop();
    void Pause();
    void Resume();
    
    void AddDsp(Scope<FmodDsp> dsp);
    
private:
    FMOD::Sound *m_Sound;
    FMOD::Channel *m_Channel;
    std::string m_Name;
    bool m_Paused;
    
    std::vector<Scope<FmodDsp>> m_Dsps;
};

}

#endif