// Copyright (c) Evangelion Manuhutu | ORigin Engine

#include "pch.h"
#include "AudioListener.h"
#include "AudioEngine.h"

namespace origin {

	AudioListener::AudioListener(const glm::vec3 &position, const glm::vec3 &velocity, const glm::vec3 &forward, const glm::vec3 &up)
	{
		Set(position, velocity, forward, up);
	}

	void AudioListener::Set(const glm::vec3& p, const glm::vec3& v, const glm::vec3& f, const glm::vec3& up)
	{
		ma_engine_listener_set_position(AudioEngine::GetEngine(), 0, p.x, p.y, p.z);
		ma_engine_listener_set_velocity(AudioEngine::GetEngine(), 0, v.x, v.y, v.z);
		ma_engine_listener_set_direction(AudioEngine::GetEngine(), 0, f.x, f.y, f.z);
		ma_engine_listener_set_world_up(AudioEngine::GetEngine(), 0, up.x, up.y, up.z);
	}

	void AudioListener::SetEnable(bool enable)
	{
		ma_engine_listener_set_enabled(AudioEngine::GetEngine(), 0, enable);
	}
}