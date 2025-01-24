// Copyright (c) 2022-present Evangelion Manuhutu | ORigin Engine

#include <Origin.hpp>

#include "Origin/Audio/FmodDsp.h"
#include "Origin/Audio/FmodSound.h"
using namespace origin;

int main()
{
    auto logger = Log();
    logger.PrintMessage("Start");

    FmodAudio::Init();
    
    Ref<FmodReverb> reverb = FmodReverb::Create();
    reverb->SetDiffusion(100.0f);
    reverb->SetWetLevel(20.0f);
    reverb->SetDecayTime(5000.0f);

    Ref<FmodDistortion> distortion = FmodDistortion::Create();
    distortion->SetDistortionLevel(1.0f);
    
    FMOD::ChannelGroup *reverb_group = FmodAudio::CreateChannelGroup("ReverbGroup");
    reverb_group->setMode(FMOD_CHANNELCONTROL_DSP_TAIL);

    reverb_group->addDSP(0, reverb->GetFmodDsp());
    
    const Ref<FmodSound> loading_screen_sound = FmodSound::Create("loading_screen", "data/loading_screen.wav");

    loading_screen_sound->SetVolume(0.1f);
    loading_screen_sound->AddToChannelGroup(reverb_group);        
    
    loading_screen_sound->Play();
    
    const Ref<FmodSound> shot = FmodSound::Create("shot", "data/shot.mp3");
    shot->SetVolume(0.5f);
    
    OGN_CORE_INFO("Sound Length MS: {}", loading_screen_sound->GetLengthMs());

    constexpr float fire_rate = 0.5f; 
    float shot_time = fire_rate;
    bool is_playing = true;
    
    auto previous_time = std::chrono::high_resolution_clock::now();
    while (true)
    {
        auto current_time = std::chrono::high_resolution_clock::now();
        std::chrono::duration<float> delta_time = current_time - previous_time;

        shot_time -= delta_time.count();
        if (shot_time <= 0.0f)
        {
            shot->Play();
            shot_time = fire_rate;
        }
        
        FmodAudio::Update(delta_time.count());
        
        previous_time = current_time;
        
        OGN_CORE_INFO("Sound position MS: {}", loading_screen_sound->GetPositionMs());

        reverb_group->isPlaying(&is_playing);
    }

    FmodAudio::Shutdown();
    
    logger.PrintMessage("Finished");
}