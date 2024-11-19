// Copyright (c) 2022-present Evangelion Manuhutu | ORigin Engine

#ifndef FMOD_DSP_H
#define FMOD_DSP_H

#include <string>

#include <fmod.hpp>
#include <fmod_common.h>
#include <fmod_dsp.h>

#include "FmodAudio.h"

namespace origin
{
struct FmodDsp
{
    FmodDsp(): m_Dsp(nullptr) {}
    ~FmodDsp()
    {
        if (m_Dsp)
        {
            m_Dsp->release();
            m_Dsp = nullptr;
        }
    }

    [[nodiscard]] FMOD::DSP *GetDspPointer() const { return m_Dsp; }
protected:
    FMOD::DSP* m_Dsp; 
};
    
struct FmodReverb : FmodDsp
{
    FmodReverb()
    {
        FMOD_CHECK(FmodAudio::GetFmodSystem()->createDSPByType(FMOD_DSP_TYPE_SFXREVERB, &m_Dsp))
    }

    void SetDecayTime(const float decay_time) const
    {
        FMOD_CHECK(m_Dsp->setParameterFloat(FMOD_DSP_SFXREVERB_DECAYTIME, decay_time))
    }
    
    [[nodiscard]] float GetDecayTime() const
    {
        float decay_time;
        FMOD_CHECK(m_Dsp->getParameterFloat(FMOD_DSP_SFXREVERB_DECAYTIME, &decay_time, nullptr, 0))
        return decay_time;
    }

    // Value in DB
    void SetWetLevel(const float wet_level) const
    {
        FMOD_CHECK(m_Dsp->setParameterFloat(FMOD_DSP_SFXREVERB_WETLEVEL, wet_level))
    }
    
    [[nodiscard]] float GetWetLevel() const
    {
        float wet_level;
        FMOD_CHECK(m_Dsp->getParameterFloat(FMOD_DSP_SFXREVERB_WETLEVEL, &wet_level, nullptr, 0))
        return wet_level;
    }
};

}

#endif