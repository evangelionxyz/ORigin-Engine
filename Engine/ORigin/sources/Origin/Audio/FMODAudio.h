// Copyright (c) 2022-present Evangelion Manuhutu | ORigin Engine

#ifndef FMOD_AUDIO_H
#define FMOD_AUDIO_H

#include "fmod.hpp"
#include "fmod_common.h"

#include <string>
#include <unordered_map>

namespace origin {

struct FMODSound
{
    FMOD::Sound *sound;
    std::string filepath;
    std::string name;
    bool paused;

    FMODSound(FMOD::Sound *sound, const std::string &name)
        : sound(sound), name(name), paused(false)
    {
    }
};

class FMODAudio
{
public:
    FMODAudio();
    ~FMODAudio();

    FMODSound *CreateSound(const std::string &name, const std::string &filepath, FMOD_MODE mode = FMOD_DEFAULT);
    FMODSound *CreateStream(const std::string &name, const std::string &filepath, FMOD_MODE mode = FMOD_DEFAULT);

    void Play(const std::string &name);
    void Play(FMODSound *fmod_sound);

    void Stop(const std::string &name);
    void Stop(FMODSound *fmod_sound);

    void Pause(const std::string &name);
    void Pause(FMODSound *fmod_sound);

    void Resume(const std::string &name);
    void Resume(FMODSound *fmod_sound);

    void SetVolume(FMODSound *fmod_sound, float volume);
    void SetChannelVolume(const std::string &name, float volume);
    void MuteChannel(const std::string &name, bool mute);

    void SetMasterVolume(float volume);
    void MuteMaster(bool mute);

    void DestroySound(const std::string &name);
    void DestroySound(FMODSound *fmod_sound);

    void Update(float ts);


    // effects
    void CreateReverb(const std::string &name);
    void CreateLowPass(const std::string &name);
    void CreateHighPass(const std::string &name);
    void AddEffectToChannel(const std::string &effect_name, const std::string &channel_name);
    void RemoveEffectFromChannel(const std::string &effect_name, const std::string &channel_name);

    // 3d audio
    void Set3DListenerPosition(const float x, const float y, const float z);
    void Set3DListenerVelocity(const float vx, const float vy, const float vz);
    void Set3DListenerOrientation(const float fx, const float fy, const float fz, const float ux, const float uy, const float uz);
    void Set3DSoundPosition(const std::string &name, const float x, const float y, const float z);
    void Set3DSoundVelocity(const std::string &name, const float vx, const float vy, const float vz);
    void Set3DSoundDistance(const std::string &name, float minDist, float maxDist);

    static FMODAudio &GetInstance();

private:
    FMOD::System *m_System;
    FMOD::ChannelGroup *m_MasterGroup;

    std::unordered_map<std::string, FMODSound *> m_SoundMap;
    std::unordered_map<std::string, FMOD::Channel *> m_ChannelMap;
    std::unordered_map<std::string, FMOD::DSP *> m_Effectes;

    FMOD_VECTOR listenerPos;
    FMOD_VECTOR listenerVel;
    FMOD_VECTOR listenerForward;
    FMOD_VECTOR listenerUp;
};

}

#endif

