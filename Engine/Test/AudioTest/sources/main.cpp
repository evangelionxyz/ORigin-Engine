// Copyright (c) 2022-present Evangelion Manuhutu | ORigin Engine

#include <Origin.h>
#include "Origin/Audio/FmodSound.h"
using namespace origin;

int main()
{
    auto logger = Log();
    logger.PrintMessage("Start");
    const FmodAudio audio_engine;
    const Ref<FmodSound> sound_a = FmodSound::Create("loading_screen", "data/loading_screen.wav");
    
    sound_a->Play();
    sound_a->SetFadeIn(0, 1500);
    sound_a->SetFadeOut(6000, sound_a->GetLengthMs());

    OGN_CORE_INFO("Sound Length MS: {}", sound_a->GetLengthMs());

    auto previousTime = std::chrono::high_resolution_clock::now();
    while (sound_a->IsPlaying())
    {
        auto currentTime = std::chrono::high_resolution_clock::now();
        std::chrono::duration<float> deltaTime = currentTime - previousTime;
        audio_engine.Update(deltaTime.count());
        previousTime = currentTime;
        
        OGN_CORE_INFO("Sound position MS: {}", sound_a->GetPositionMs());
    }
    logger.PrintMessage("Finished");
}