// Copyright (c) Evangelion Manuhutu | ORigin Engine

#include "pch.h"

#define MINIAUDIO_IMPLEMENTATION
#include "AudioEngine.h"

namespace origin
{
    struct AudioEngineData
    {
        ma_engine *Engine;
        ma_engine_config Config;
    };
    static AudioEngineData s_AudioData;

    void AudioEngine::Init()
    {
        s_AudioData.Config = ma_engine_config_init();
        s_AudioData.Config.channels = 2;
        s_AudioData.Config.sampleRate = 48000;
        s_AudioData.Config.noDevice = MA_FALSE;

        s_AudioData.Engine = new ma_engine();
        ma_result result = ma_engine_init(&s_AudioData.Config, s_AudioData.Engine);
        OGN_CORE_ASSERT(result == MA_SUCCESS, "[Miniaudio Engine] Failed To Initialized");
        OGN_CORE_INFO("[Miniaudio Engine] Initialized");
    }

    void AudioEngine::Shutdown()
    {
        ma_engine_uninit(s_AudioData.Engine);
        
        delete s_AudioData.Engine;
        s_AudioData.Engine = nullptr;

        OGN_CORE_WARN("[Miniaudio Engine] Shutdown");
    }

    ma_engine *AudioEngine::GetEngine()
    {
        return s_AudioData.Engine;
    }

    void AudioEngine::CreateSound(const char *name, const char *filepath, ma_sound* sound)
    {
        ma_result result = ma_sound_init_from_file(s_AudioData.Engine, filepath, 0, nullptr, nullptr, sound);
        OGN_CORE_ASSERT(result == MA_SUCCESS, "[Miniaudio Engine] Failed to create sound");
    }

    void AudioEngine::CreateStreaming(const char *name, const char *filepath, ma_sound* sound)
    {
        ma_sound_init_from_file(s_AudioData.Engine, filepath, MA_SOUND_FLAG_STREAM, nullptr, nullptr, sound);
    }
}