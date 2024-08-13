// Copyright (c) Evangelion Manuhutu | ORigin Engine
#pragma once

#include <string>
#include "miniaudio.h"
#include "Origin/Asset/Asset.h"
#include "Origin/Scene/Components/Components.h"

namespace origin {

	class AudioEngine
	{
	public:
		static void Init();
		static void Shutdown();
		static ma_engine *GetEngine();
		static void CreateSound(const char *name, const char *filepath, ma_sound *sound);
		static void CreateStreaming(const char *name, const char *filepath, ma_sound* sound);
	};
}

