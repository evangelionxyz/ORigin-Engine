// Copyright (c) 2022-present Evangelion Manuhutu | ORigin Engine

#include <Origin.h>

#include "Origin/Audio/FmodDsp.h"
#include "Origin/Audio/FmodSound.h"
using namespace origin;

int main()
{
    auto logger = Log();
    logger.PrintMessage("Start");
    const FmodAudio audio_engine;

    Ref<FmodReverb> reverb = FmodReverb::Create();
    reverb->SetDiffusion(100.0f);
    reverb->SetWetLevel(20.0f);
    reverb->SetDecayTime(10000.0f);

    Ref<FmodDistortion> distortion = FmodDistortion::Create();
    distortion->SetDistortionLevel(0.2f);
    
    const Ref<FmodSound> sound = FmodSound::Create("loading_screen", "data/loading_screen.wav", FMOD_DEFAULT);
    sound->Play();
    sound->SetMode(FMOD_CHANNELCONTROL_DSP_TAIL);

    sound->AddDsp(reverb->GetFmodDsp());
    sound->AddDsp(reverb->GetFmodDsp());
    
    //sound_a->SetFadeIn(0, 1500);
    //sound_a->SetFadeOut(6000, sound_a->GetLengthMs());

    OGN_CORE_INFO("Sound Length MS: {}", sound->GetLengthMs());

    auto previous_time = std::chrono::high_resolution_clock::now();
    while (true)
    {
        auto current_time = std::chrono::high_resolution_clock::now();
        std::chrono::duration<float> delta_time = current_time - previous_time;
        audio_engine.Update(delta_time.count());
        previous_time = current_time;
        
        OGN_CORE_INFO("Sound position MS: {}", sound->GetPositionMs());
    }
    
    logger.PrintMessage("Finished");
}