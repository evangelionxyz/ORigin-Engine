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

    void SetActive(bool active) const;
    bool IsActive() const;

    [[nodiscard]] FMOD::DSP *GetFmodDsp() const { return m_Dsp; }
protected:
    FMOD::DSP* m_Dsp; 
};
    
struct FmodReverb : FmodDsp
{
    FmodReverb();

    /// @param millis Units: milliseconds
    /// range[100, 2000], default [1500]
    /// Reverberation decay time at low-frequency
    void SetDecayTime(const float millis) const;
    [[nodiscard]] float GetDecayTime() const;

    /// @param millis Units: milliseconds 
    /// range[0, 300], default [20]
    /// Delay time of first reflection
    void SetEarlyDelay(float millis) const;
    [[nodiscard]] float GetEarlyDelay() const;

    /// @param millis Units: milliseconds
    /// range[0, 300], default [40]
    /// late reverberation dela time relative
    /// to first reflection in milliseconds
    void SetLateDelay(float millis) const;
    [[nodiscard]] float GetLateDelay() const;

    /// @param hertz Units: hertz
    /// range[20, 20000], default [5000]
    /// Reference high frequency for high-frequency decay
    void SetHighFrequencyReference(const float hertz) const;
    [[nodiscard]] float GetHighFrequencyReference() const;

    /// @param percents Units: percents
    /// range[10, 100], default [50]
    /// Reverberation diffusion [echo density]
    void SetDiffusion(const float percents) const;
    [[nodiscard]] float GetDiffusion() const;
    
    /// @param percents Units: percents
    /// range[10, 100], default [50]
    /// Reverberation density [modal density]
    void SetDensity(float percents) const;
    [[nodiscard]] float GetDensity() const;

    /// @param decibels Units: Decibels
    /// range[20, 1000], default [250]
    /// Gain of low-self filter
    void SetLowShelfGain(const float decibels) const;
    [[nodiscard]] float GetLowShelfGain() const;

    /// @param hertz Units: Hertz
    /// range[20, 20000], default [20000]
    /// Cutoff frequency of low-pass filter
    void SetHighCut(const float hertz) const;
    [[nodiscard]] float GetHighCut() const;
    
    /// @param decibels Units: Decibels
    /// range[-80, 20], default [0]
    /// Dry signal level
    void SetDryLevel(const float decibels) const;
    [[nodiscard]] float GetDryLevel() const;
    
    
    /// @param decibels Units: Decibels
    /// range[-80, 20], default [-6]
    /// Reverb signal level
    void SetWetLevel(const float decibels) const;
    [[nodiscard]] float GetWetLevel() const;

    static Ref<FmodReverb> Create();
};

struct FmodDistortion : FmodDsp
{
    FmodDistortion();

    /// @param distortion_level Units: Linear
    /// range [0, 1], default [0.5]
    /// Distortion value
    void SetDistortionLevel(const float distortion_level) const;
    [[nodiscard]] float GetDistortionLevel() const;
    static Ref<FmodDistortion> Create();
};

}

#endif