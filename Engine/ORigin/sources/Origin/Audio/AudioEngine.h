// Copyright (c) 2022-present Evangelion Manuhutu | ORigin Engine

#ifndef AUDIO_ENGINE_H
#define AUDIO_ENGINE_H

#include "miniaudio.h"
#include "Origin/Scene/Components/Components.h"
#include <string>

namespace origin {

enum AudioAPI
{
    Miniaudio = 0,
    FMOD = 1
};

class AudioEngine
{
public:
    static void Init();
    static void Shutdown();
    static ma_engine *GetEngine();
    static void CreateSound(const char *name, const char *filepath, ma_sound *sound);
    static void CreateStreaming(const char *name, const char *filepath, ma_sound *sound);
};

}

#endif