
#include "AudioSource.h"

namespace origin
{
	void AudioSource::Play()
	{
		ma_sound_start(m_Sound.get());
	}

	void AudioSource::Pause()
	{
		ma_sound_stop(m_Sound.get());
	}

	void AudioSource::Stop()
	{
		ma_sound_stop(m_Sound.get());
		ma_sound_seek_to_pcm_frame(m_Sound.get(), 0);
	}

	void AudioSource::SetVolume(float volume)
	{
		ma_sound_set_volume(m_Sound.get(), volume);
	}

	void AudioSource::SetPitch(float pitch)
	{
		ma_sound_set_pitch(m_Sound.get(), pitch);
	}

	void AudioSource::SetPaning(float pan)
	{
		ma_sound_set_pan(m_Sound.get(), pan);
	}

	void AudioSource::SetPosition(const glm::vec3 &position, ma_positioning mode)
	{
		ma_sound_set_position(m_Sound.get(), position.x, position.y, position.z);
		ma_sound_set_positioning(m_Sound.get(), mode);
	}

	bool AudioSource::IsPlaying()
	{
		return ma_sound_is_playing(m_Sound.get()) != 0;
	}

	bool AudioSource::IsPaused()
	{
		return ma_sound_is_playing(m_Sound.get()) == 0;
	}

	bool AudioSource::IsLooping()
	{
		return ma_sound_is_looping(m_Sound.get());
	}

	bool AudioSource::IsSpatial()
	{
		return ma_sound_is_spatialization_enabled(m_Sound.get());
	}

	float AudioSource::GetVolume()
	{
		return ma_sound_get_volume(m_Sound.get());
	}

	float AudioSource::GetPitch()
	{
		return ma_sound_get_pitch(m_Sound.get());
	}

	float AudioSource::GetMinDistance()
	{
		return ma_sound_get_min_distance(m_Sound.get());
	}

	float AudioSource::GetMaxDistance()
	{
		return ma_sound_get_max_distance(m_Sound.get());
	}

	const glm::vec3 AudioSource::GetPosition() const
	{
		ma_vec3f p = ma_sound_get_position(m_Sound.get());
		return { p.x, p.y, p.z };
	}

	void AudioSource::SetLoop(bool loop)
	{
		ma_sound_set_looping(m_Sound.get(), loop);
	}

	void AudioSource::SetName(const char* name)
	{
		m_Name = name;
	}

	void AudioSource::SetSpatial(bool enable)
	{
		ma_sound_set_spatialization_enabled(m_Sound.get(), enable);
	}

	void AudioSource::SetMinMaxDistance(float minVal, float maxVal)
	{
		ma_sound_set_min_distance(m_Sound.get(), minVal);
		ma_sound_set_max_distance(m_Sound.get(), maxVal);
	}

	void AudioSource::LoadSource(const char *name, const std::filesystem::path &filepath, bool looping, bool spatializing)
	{
		m_Sound = std::make_unique<ma_sound>();
		AudioEngine::CreateSound(name, filepath.generic_string().c_str(), m_Sound.get());
		SetLoop(looping);
		SetSpatial(spatializing);
		if (spatializing)
		{
			float defMin = 10.0f;
			float defMax = 20.0f;
			SetMinMaxDistance(defMin, defMax);
		}

		IsLoaded = true;
	}

	void AudioSource::LoadStreamingSource(const char *name, const std::filesystem::path &filepath, bool looping, bool spatializing)
	{
		m_Sound = std::make_unique<ma_sound>();
		AudioEngine::CreateStreaming(name, filepath.generic_string().c_str(), m_Sound.get());
		SetLoop(looping);
		SetSpatial(spatializing);
		if (spatializing)
		{
			float defMin = 10.0f;
			float defMax = 20.0f;
			SetMinMaxDistance(defMin, defMax);
		}

		IsLoaded = true;
	}

	std::shared_ptr<AudioSource> AudioSource::Create()
	{
		return std::make_shared<AudioSource>();
	}

}