// Copyright (c) Evangelion Manuhutu | ORigin Engine

#include "pch.h"

#define MINIAUDIO_IMPLEMENTATION
#include "AudioEngine.h"

namespace origin {

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
		OGN_CORE_ASSERT(result == MA_SUCCESS, "[MiniaudioEngine] Failed To Initialized");
		OGN_CORE_INFO("[MiniaudioEngine] Initialized");
	}

	void AudioEngine::Shutdown()
	{
		ma_engine_uninit(s_AudioData.Engine);
		
		delete s_AudioData.Engine;
		s_AudioData.Engine = nullptr;

		OGN_CORE_WARN("[MiniaudioEngine] Shutdown");
	}

	ma_engine *AudioEngine::GetEngine()
	{
		return s_AudioData.Engine;
	}

	void AudioEngine::CreateSound(const char *name, const char *filepath, ma_sound* sound)
	{
		ma_result result = ma_sound_init_from_file(s_AudioData.Engine, filepath, 0, nullptr, nullptr, sound);
		OGN_CORE_ASSERT(result == MA_SUCCESS, "[MiniaudioEngine] Failed create sound");
	}

	void AudioEngine::CreateStreaming(const char *name, const char *filepath, ma_sound* sound)
	{
		ma_sound_init_from_file(s_AudioData.Engine, filepath, MA_SOUND_FLAG_STREAM, nullptr, nullptr, sound);
	}

	void AudioEngine::SetListener(const glm::vec3 &p, const glm::vec3 &v, const glm::vec3 &f, const glm::vec3 &up)
	{
		ma_engine_listener_set_position(s_AudioData.Engine, 0, p.x, p.y, p.z);
		ma_engine_listener_set_velocity(s_AudioData.Engine, 0, v.x, v.y, v.z);
		ma_engine_listener_set_direction(s_AudioData.Engine, 0, f.x, f.y, f.z);
		ma_engine_listener_set_world_up(s_AudioData.Engine, 0, up.x, up.y, up.z);
	}

}