// Copyright (c) 2022-present Evangelion Manuhutu | ORigin Engine

#ifndef FMOD_AUDIO_H
#define FMOD_AUDIO_H

#include <fmod.hpp>
#include <fmod_common.h>
#include <string>
#include <unordered_map>

#define FMOD_CHECK(x) if ((x) != FMOD_OK) __debugbreak();

namespace origin {

struct FmodDsp;
struct FmodSound;

class FmodAudio
{
public:
    FmodAudio();
    ~FmodAudio();

    void SetMasterVolume(float volume) const;
    void MuteMaster(bool mute) const;

    void Update(float deltaTime) const;
    static FmodAudio &GetInstance();
    static FMOD::System *GetFmodSystem();
    static FMOD::ChannelGroup *GetFmodChannelGroup();
    static void InsertFmodSound(const std::string &name, Ref<FmodSound> sound);

private:
    FMOD::System *m_System;
    FMOD::ChannelGroup *m_MasterGroup;
    std::unordered_map<std::string, Ref<FmodSound>> m_SoundMap;

    FMOD_VECTOR listenerPos;
    FMOD_VECTOR listenerVel;
    FMOD_VECTOR listenerForward;
    FMOD_VECTOR listenerUp;
};

}

#endif

