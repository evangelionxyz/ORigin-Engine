// Copyright (c) Evangelion Manuhutu | ORigin Engine

#include "pch.h"
#define MINIAUDIO_IMPLEMENTATION
#include "AudioEngine.h"

namespace origin {

	struct AudioEngineData
	{
		std::unique_ptr<ma_engine> Engine;
		ma_engine_config Config;
	};

	static AudioEngineData s_Data;

	void AudioEngine::Init()
	{
		s_Data.Config =	ma_engine_config_init();
		s_Data.Config.channels = 2;
		s_Data.Config.sampleRate = 41000;
		s_Data.Config.noDevice = MA_FALSE;

		s_Data.Engine = std::make_unique<ma_engine>();
		if (ma_engine_init(&s_Data.Config, s_Data.Engine.get()) != MA_SUCCESS)
			OGN_CORE_ASSERT(false, "Miniaudio Engine: Failed To Initialized");

		OGN_CORE_INFO("Miniaudio Engine: Initialized");
	}

	void AudioEngine::Shutdown()
	{
		ma_engine_uninit(s_Data.Engine.get());
		OGN_CORE_WARN("Miniaudio Engine: Shutdown");
	}

	ma_engine *AudioEngine::GetEngine()
	{
		return s_Data.Engine.get();
	}

	void AudioEngine::CreateSound(const char *name, const char *filepath, ma_sound* sound)
	{
		ma_sound_init_from_file(s_Data.Engine.get(), filepath, 0, nullptr, nullptr, sound);
	}

	void AudioEngine::CreateStreaming(const char *name, const char *filepath, ma_sound* sound)
	{
		ma_sound_init_from_file(s_Data.Engine.get(), filepath, MA_SOUND_FLAG_STREAM, nullptr, nullptr, sound);
	}

	void AudioEngine::SetListener(const glm::vec3 &p, const glm::vec3 &v, const glm::vec3 &f, const glm::vec3 &up)
	{
		ma_engine_listener_set_position(s_Data.Engine.get(), 0, p.x, p.y, p.z);
		ma_engine_listener_set_velocity(s_Data.Engine.get(), 0, v.x, v.y, v.z);
		ma_engine_listener_set_direction(s_Data.Engine.get(), 0, f.x, f.y, f.z);
		ma_engine_listener_set_world_up(s_Data.Engine.get(), 0, up.x, up.y, up.z);
	}

}