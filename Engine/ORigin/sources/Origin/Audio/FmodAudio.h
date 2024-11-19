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

    Ref<FmodSound> CreateSound(const std::string &name, const std::string &filepath, FMOD_MODE mode = FMOD_DEFAULT);
    Ref<FmodSound> CreateStream(const std::string &name, const std::string &filepath, FMOD_MODE mode = FMOD_DEFAULT);

    void SetMasterVolume(float volume) const;
    void MuteMaster(bool mute) const;

    void Update() const;
    static FmodAudio &GetInstance();
    static FMOD::System *GetFmodSystem();

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

