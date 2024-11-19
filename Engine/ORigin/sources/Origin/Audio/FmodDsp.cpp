// Copyright (c) 2022-present Evangelion Manuhutu | ORigin Engine

#include "pch.h"
#include "FmodDsp.h"

namespace origin {

void FmodDsp::SetActive(const bool active) const
{
    m_Dsp->setActive(active);    
}

bool FmodDsp::IsActive() const
{
    bool active = false;
    m_Dsp->getActive(&active);
    return active;
}
// ====================================
// Reverb
// ====================================
FmodReverb::FmodReverb()
{
    FMOD_RESULT result = FmodAudio::GetFmodSystem()->createDSPByType(FMOD_DSP_TYPE_SFXREVERB, &m_Dsp);
    FMOD_CHECK(result)
}
void FmodReverb::SetDecayTime(const float millis) const
{
    FMOD_CHECK(m_Dsp->setParameterFloat(FMOD_DSP_SFXREVERB_DECAYTIME, millis))
}
float FmodReverb::GetDecayTime() const
{
    float v = 0.0f;
    m_Dsp->getParameterFloat(FMOD_DSP_SFXREVERB_DECAYTIME, &v, nullptr, 0);
    return v;
}

void FmodReverb::SetEarlyDelay(float millis) const
{
    FMOD_CHECK(m_Dsp->setParameterFloat(FMOD_DSP_SFXREVERB_EARLYDELAY, millis))
}

float FmodReverb::GetEarlyDelay() const
{
    float v = 0.0f;
    m_Dsp->getParameterFloat(FMOD_DSP_SFXREVERB_EARLYDELAY, &v, nullptr, 0);
    return v;
}

void FmodReverb::SetLateDelay(float millis) const
{
    FMOD_CHECK(m_Dsp->setParameterFloat(FMOD_DSP_SFXREVERB_LATEDELAY, millis))
}

float FmodReverb::GetLateDelay() const
{
    float v = 0.0f;
    m_Dsp->getParameterFloat(FMOD_DSP_SFXREVERB_LATEDELAY, &v, nullptr, 0);
    return v;
}

void FmodReverb::SetHighFrequencyReference(const float hertz) const
{
    FMOD_CHECK(m_Dsp->setParameterFloat(FMOD_DSP_SFXREVERB_HFREFERENCE, hertz))
}
float FmodReverb::GetHighFrequencyReference() const
{
    float v = 0.0f;
    m_Dsp->getParameterFloat(FMOD_DSP_SFXREVERB_HFREFERENCE, &v, nullptr, 0);
    return v;
}

void FmodReverb::SetDiffusion(const float percents) const
{
    FMOD_CHECK(m_Dsp->setParameterFloat(FMOD_DSP_SFXREVERB_DIFFUSION, percents))
}

float FmodReverb::GetDiffusion() const
{
    float v = 0.0f;
    m_Dsp->getParameterFloat(FMOD_DSP_SFXREVERB_DIFFUSION, &v, nullptr, 0);
    return v;
}

void FmodReverb::SetDensity(float percents) const
{
    FMOD_CHECK(m_Dsp->setParameterFloat(FMOD_DSP_SFXREVERB_DENSITY, percents))
}

float FmodReverb::GetDensity() const
{
    float v = 0.0f;
    m_Dsp->getParameterFloat(FMOD_DSP_SFXREVERB_DENSITY, &v, nullptr, 0);
    return v;
}

void FmodReverb::SetLowShelfGain(const float decibels) const
{
    FMOD_CHECK(m_Dsp->setParameterFloat(FMOD_DSP_SFXREVERB_LOWSHELFFREQUENCY, decibels))
}

float FmodReverb::GetLowShelfGain() const
{
    float v = 0.0f;
    m_Dsp->getParameterFloat(FMOD_DSP_SFXREVERB_LOWSHELFFREQUENCY, &v, nullptr, 0);
    return v;
}

void FmodReverb::SetHighCut(const float hertz) const
{
    FMOD_CHECK(m_Dsp->setParameterFloat(FMOD_DSP_SFXREVERB_HIGHCUT, hertz))
}

float FmodReverb::GetHighCut() const
{
    float v = 0.0f;
    m_Dsp->getParameterFloat(FMOD_DSP_SFXREVERB_DRYLEVEL, &v, nullptr, 0);
    return v;
}

void FmodReverb::SetDryLevel(const float decibels) const
{
    FMOD_CHECK(m_Dsp->setParameterFloat(FMOD_DSP_SFXREVERB_DRYLEVEL, decibels))
}

float FmodReverb::GetDryLevel() const
{
    float v = 0.0f;
    m_Dsp->getParameterFloat(FMOD_DSP_SFXREVERB_EARLYLATEMIX, &v, nullptr, 0);
    return v;
}

void FmodReverb::SetWetLevel(const float decibels) const
{
    FMOD_CHECK(m_Dsp->setParameterFloat(FMOD_DSP_SFXREVERB_WETLEVEL, decibels))
}

float FmodReverb::GetWetLevel() const
{
    float v = 0.0f;
    m_Dsp->getParameterFloat(FMOD_DSP_SFXREVERB_WETLEVEL, &v, nullptr, 0);
    return v;
}

Ref<FmodReverb> FmodReverb::Create()
{
    return CreateRef<FmodReverb>();
}


// ====================================
// Distortion
// ====================================
FmodDistortion::FmodDistortion()
{
    FMOD_RESULT result = FmodAudio::GetFmodSystem()->createDSPByType(FMOD_DSP_TYPE_DISTORTION, &m_Dsp);
    FMOD_CHECK(result)
}

void FmodDistortion::SetDistortionLevel(const float value) const
{
    FMOD_CHECK(m_Dsp->setParameterFloat(FMOD_DSP_DISTORTION_LEVEL, value))
}

float FmodDistortion::GetDistortionLevel() const
{
    float v = 0.0f;
    m_Dsp->getParameterFloat(FMOD_DSP_DISTORTION_LEVEL, &v, nullptr, 0);
    return v;
}

Ref<FmodDistortion> FmodDistortion::Create()
{
    return CreateRef<FmodDistortion>();
}

}
