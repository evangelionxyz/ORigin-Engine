// Copyright (c) 2022-present Evangelion Manuhutu | ORigin Engine

#ifndef FMOD_AUDIO_H
#define FMOD_AUDIO_H

#include <fmod.hpp>
#include <fmod_common.h>
#include <string>
#include <unordered_map>

#define FMOD_CHECK(x) if ((x) != FMOD_OK) OGN_DEBUGBREAK();

namespace origin {

struct FmodDsp;
struct FmodSound;

class FmodAudio
{
private:
    FmodAudio() = default;
    ~FmodAudio() = default;
public:
    static void SetMasterVolume(float volume);
    static void MuteMaster(bool mute);

    static void Init();
    static void Shutdown();
    
    static void Update(float delta_time);
    
    static FMOD::ChannelGroup *CreateChannelGroup(const std::string &name);
    static std::unordered_map<std::string, FMOD::ChannelGroup *> GetChannelGroupMap();
    static FMOD::ChannelGroup *GetChannelGroup(const std::string &name);
    static FmodAudio &GetInstance();
    static FMOD::System *GetFmodSystem();
    static FMOD::ChannelGroup *GetMasterChannel();
    static float GetMasterVolume();
    static void InsertFmodSound(const std::string &name, const Ref<FmodSound>& sound);
    static void RemoveFmodSound(const std::string &name);

private:
    FMOD::System *m_System;
    FMOD::ChannelGroup *m_MasterGroup;
    std::unordered_map<std::string, FMOD::ChannelGroup *> m_ChannelGroups;
    std::unordered_map<std::string, Ref<FmodSound>> m_SoundMap;

    FMOD_VECTOR listenerPos;
    FMOD_VECTOR listenerVel;
    FMOD_VECTOR listenerForward;
    FMOD_VECTOR listenerUp;
};

}

#endif

