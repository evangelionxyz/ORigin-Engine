#include "pch.h"
#include "FMODAudio.h"

#define FMOD_CHECK(x) if (x != FMOD_OK) __debugbreak();

namespace origin {

static FMOD_RESULT fmod_result;
static FMODAudio *s_FMODAudio = nullptr;

FMODAudio::FMODAudio()
{
    s_FMODAudio = this;

    fmod_result = FMOD::System_Create(&m_System);
    FMOD_CHECK(fmod_result);

    fmod_result = m_System->init(32, FMOD_INIT_NORMAL, nullptr);
    FMOD_CHECK(fmod_result);

    fmod_result = m_System->createChannelGroup("master", &m_MasterGroup);
    FMOD_CHECK(fmod_result);

    // Initialize listener position
    listenerPos = {0.0f, 0.0f, 0.0f};
    listenerVel = {0.0f, 0.0f, 0.0f};
    listenerForward = {0.0f, 0.0f, 1.0f};
    listenerUp = {0.0f, 1.0f, 0.0f};
}

FMODAudio::~FMODAudio()
{
    // destroy sounds
    for (auto &[name, sound] : m_SoundMap)
    {
        sound->sound->release();
        delete sound;
    }

    m_SoundMap.clear();

    fmod_result = m_System->close();
    FMOD_CHECK(fmod_result);

    fmod_result = m_System->release();
    FMOD_CHECK(fmod_result);
}

FMODSound *FMODAudio::CreateSound(const std::string &name, const std::string &filepath, FMOD_MODE mode)
{
    FMOD::Sound *sound = nullptr;
    m_System->createSound(filepath.c_str(), mode, nullptr, &sound);
    FMODSound *fmod_sound = new FMODSound(sound, name);

    m_SoundMap.insert({ name, fmod_sound });

    return fmod_sound;
}

FMODSound *FMODAudio::CreateStream(const std::string &name, const std::string &filepath, FMOD_MODE mode)
{
    FMOD::Sound *stream = nullptr;
    m_System->createStream(filepath.c_str(), mode, nullptr, &stream);
    FMODSound *fmod_sound = new FMODSound(stream, name);

    m_SoundMap.insert({ name, fmod_sound });

    return fmod_sound;
}

void FMODAudio::Play(const std::string &name)
{
    auto it = m_SoundMap.find(name);
    if (it != m_SoundMap.end())
    {
        FMOD::Channel *channel;
        fmod_result = m_System->playSound(it->second->sound, m_MasterGroup,
            it->second->paused, &channel);
        FMOD_CHECK(fmod_result);
        m_ChannelMap[name] = channel;
    }
}

void FMODAudio::Play(FMODSound *fmod_sound)
{
    if (fmod_sound->sound)
    {
        FMOD::Channel *channel;
        fmod_result = m_System->playSound(fmod_sound->sound, m_MasterGroup,
            fmod_sound->paused, &channel);
        FMOD_CHECK(fmod_result);
        m_ChannelMap[fmod_sound->name] = channel;
    }
}

void FMODAudio::Stop(const std::string &name)
{
    auto channel_it = m_ChannelMap.find(name);
    if (channel_it != m_ChannelMap.end())
    {
        channel_it->second->stop();
    }
}

void FMODAudio::Stop(FMODSound *fmod_sound)
{
    auto channel_it = m_ChannelMap.find(fmod_sound->name);
    if (channel_it != m_ChannelMap.end())
    {
        channel_it->second->stop();
    }
}

void FMODAudio::Pause(const std::string &name)
{
    auto channel_it = m_ChannelMap.find(name);
    if (channel_it != m_ChannelMap.end())
    {
        channel_it->second->setPaused(true);
    }
}

void FMODAudio::Pause(FMODSound *fmod_sound)
{
    auto channel_it = m_ChannelMap.find(fmod_sound->name);
    if (channel_it != m_ChannelMap.end())
    {
        channel_it->second->setPaused(true);
    }
}


void FMODAudio::Resume(const std::string &name)
{
    auto channel_it = m_ChannelMap.find(name);
    if (channel_it != m_ChannelMap.end())
    {
        channel_it->second->setPaused(false);
    }
}

void FMODAudio::Resume(FMODSound *fmod_sound)
{
    auto channel_it = m_ChannelMap.find(fmod_sound->name);
    if (channel_it != m_ChannelMap.end())
    {
        channel_it->second->setPaused(false);
    }
}


void FMODAudio::SetVolume(FMODSound *fmod_sound, float volume)
{
    auto channel_it = m_ChannelMap.find(fmod_sound->name);
    if (channel_it != m_ChannelMap.end())
    {
        channel_it->second->setVolume(volume);
    }
}

void FMODAudio::SetMasterVolume(float volume)
{
    m_MasterGroup->setVolume(volume);
}

void FMODAudio::SetChannelVolume(const std::string &name, float volume)
{
    auto channel_it = m_ChannelMap.find(name);
    if (channel_it != m_ChannelMap.end())
    {
        channel_it->second->setVolume(volume);
    }
}

void FMODAudio::MuteChannel(const std::string &name, bool mute)
{
    auto channel_it = m_ChannelMap.find(name);
    if (channel_it != m_ChannelMap.end())
    {
        channel_it->second->setMute(mute);
    }
}

void FMODAudio::MuteMaster(bool mute)
{
    m_MasterGroup->setMute(mute);
}

void FMODAudio::DestroySound(const std::string &name)
{
    auto it = m_SoundMap.find(name);
    if (it != m_SoundMap.end())
    {
        it->second->sound->release();
        delete it->second;
        m_SoundMap.erase(name);
    }
}

void FMODAudio::DestroySound(FMODSound *fmod_sound)
{
    fmod_sound->sound->release();
    auto it = m_SoundMap.find(fmod_sound->name);
    if (it != m_SoundMap.end())
    {
        delete it->second;
        m_SoundMap.erase(fmod_sound->name);
    }
}

void FMODAudio::Update(float ts)
{
    fmod_result = m_System->update();
    FMOD_CHECK(fmod_result);
}
    
// EFFECTS
void FMODAudio::CreateReverb(const std::string &name)
{
    FMOD::DSP *reverb;
    fmod_result = m_System->createDSPByType(FMOD_DSP_TYPE_SFXREVERB, &reverb);
    FMOD_CHECK(fmod_result);
    m_Effects[name] = reverb;
}

void FMODAudio::CreateLowPass(const std::string &name)
{
    FMOD::DSP *low_pass;
    fmod_result = m_System->createDSPByType(FMOD_DSP_TYPE_LOWPASS, &low_pass);
    FMOD_CHECK(fmod_result);
    m_Effects[name] = low_pass;
}

void FMODAudio::CreateHighPass(const std::string &name)
{
    FMOD::DSP *high_pass;
    fmod_result = m_System->createDSPByType(FMOD_DSP_TYPE_HIGHPASS, &high_pass);
    FMOD_CHECK(fmod_result);
    m_Effects[name] = high_pass;
}

void FMODAudio::SetEffectParameter(const std::string &effect_name, int parameter, float value)
{
    auto effect_it = m_Effects.find(effect_name);
    if (effect_it != m_Effects.end())
    {
        effect_it->second->setParameterFloat(parameter, value);
    }
}

void FMODAudio::AddEffectToChannel(const std::string &effect_name, const std::string &channel_name)
{
    auto effect_it = m_Effects.find(effect_name);
    auto channel_it = m_ChannelMap.find(effect_name);

    if (effect_it != m_Effects.end() && channel_it != m_ChannelMap.end())
    {
        channel_it->second->addDSP(0, effect_it->second);
    }
}

void FMODAudio::RemoveEffectFromChannel(const std::string &effect_name, const std::string &channel_name)
{
    auto effect_it = m_Effects.find(effect_name);
    auto channel_it = m_ChannelMap.find(effect_name);

    if (effect_it != m_Effects.end() && channel_it != m_ChannelMap.end())
    {
        channel_it->second->removeDSP(effect_it->second);
    }
}

    
// 3D Audio
void FMODAudio::Set3DListenerPosition(const float x, const float y, const float z)
{
}

void FMODAudio::Set3DListenerVelocity(const float vx, const float vy, const float vz)
{
}

void FMODAudio::Set3DListenerOrientation(const float fx, const float fy, const float fz, const float ux, const float uy, const float uz)
{
}

void FMODAudio::Set3DSoundPosition(const std::string &name, const float x, const float y, const float z)
{
}

void FMODAudio::Set3DSoundVelocity(const std::string &name, const float vx, const float vy, const float vz)
{
}

void FMODAudio::Set3DSoundDistance(const std::string &name, float minDist, float maxDist)
{
}

FMODAudio &FMODAudio::GetInstance()
{
    return *s_FMODAudio;
}

}
