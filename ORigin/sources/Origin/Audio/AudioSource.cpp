// Copyright (c) Evangelion Manuhutu | ORigin Engine

#include "pch.h"
#include "AudioSource.h"
#include "Origin/Core/ConsoleManager.h"

namespace origin
{
    void AudioSource::Play()
    {
        if (ma_sound_is_playing(m_Sounds[0]))
        {
            return;
        }

        ma_result result = ma_sound_start(m_Sounds[0]);
        if (result != MA_SUCCESS)
        {
            OGN_CORE_ERROR("[Audio Source] Failed to start sound: {0}", result);
            PUSH_CONSOLE_ERROR("[Audio Source] Failed to start sound: {0}", result);
            return;
        }
    }

	void AudioSource::Pause()
	{
		for (ma_sound *sound : m_Sounds)
		{
			ma_sound_stop(sound);
		}
	}

	void AudioSource::Stop()
	{
		for (ma_sound *sound : m_Sounds)
		{
			ma_sound_stop(sound);
			ma_sound_seek_to_pcm_frame(sound, 0);
		}
	}

    void AudioSource::PlayLooped()
    {
		ma_sound_set_looping(m_Sounds[0], MA_TRUE);
		ma_sound_start(m_Sounds[0]);
    }

    void AudioSource::PlayOverlapping()
    {
		if (!m_OverlappingAllowed)
		{
            OGN_CORE_ERROR("[Audio Source] Sound overlapping is not allowed", m_Name);
			PUSH_CONSOLE_ERROR("[Audio Source] Sound overlapping is not allowed", m_Name);
			return;
		}

        ma_result result = ma_sound_start(m_Sounds[m_OverlapIndex]);
        if (result != MA_SUCCESS)
        {
            OGN_CORE_ERROR("[Audio Source] Failed to start overlapping: {0}", result);
            PUSH_CONSOLE_ERROR("[Audio Source] Failed to start overlapping: {0}", result);
        }

        m_OverlapIndex++;
        if (m_OverlapIndex >= m_MaxOverlap - 1)
        {
            m_OverlapIndex = 0;
        }
    }

    void AudioSource::SetVolume(float volume)
    {
        for (ma_sound *sound : m_Sounds)
        {
            ma_sound_set_volume(sound, volume);
        }
    }

    void AudioSource::SetPitch(float pitch)
    {
        for (ma_sound *sound : m_Sounds)
        {
            ma_sound_set_pitch(sound, pitch);
        }
    }

    void AudioSource::SetPaning(float pan)
    {
        for (ma_sound *sound : m_Sounds)
        {
            ma_sound_set_pan(sound, pan);
        }
    }

    void AudioSource::SetPosition(const glm::vec3 &position, int index, ma_positioning mode)
    {
        if (index == -1)
        {
            for (ma_sound *sound : m_Sounds)
            {
                ma_sound_set_position(sound, position.x, position.y, position.z);
                ma_sound_set_positioning(sound, mode);
            }
        }
        else
        {
            ma_sound_set_position(m_Sounds[index], position.x, position.y, position.z);
            ma_sound_set_positioning(m_Sounds[index], mode);
        }
    }

    bool AudioSource::IsPlaying()
	{
		return ma_sound_is_playing(m_Sounds[0]) != 0;
	}

	bool AudioSource::IsPaused()
	{
		return ma_sound_is_playing(m_Sounds[0]) == 0;
	}

	bool AudioSource::IsLooping()
	{
		return ma_sound_is_looping(m_Sounds[0]);
	}

	bool AudioSource::IsSpatial()
	{
		return ma_sound_is_spatialization_enabled(m_Sounds[0]);
	}

	float AudioSource::GetVolume()
	{
		return ma_sound_get_volume(m_Sounds[0]);
	}

	float AudioSource::GetPitch()
	{
		return ma_sound_get_pitch(m_Sounds[0]);
	}

	float AudioSource::GetMinDistance()
	{
		return ma_sound_get_min_distance(m_Sounds[0]);
	}

	float AudioSource::GetMaxDistance()
	{
		return ma_sound_get_max_distance(m_Sounds[0]);
	}

	const glm::vec3 AudioSource::GetPosition(int index) const
	{
		ma_vec3f p = ma_sound_get_position(m_Sounds[index]);
		return { p.x, p.y, p.z };
	}

	void AudioSource::SetLoop(bool loop)
	{
		for (ma_sound *sound : m_Sounds)
		{
			ma_sound_set_looping(sound, loop);
		}
	}

	void AudioSource::SetName(const char* name)
	{
		m_Name = name;
	}

	void AudioSource::SetSpatial(bool enable)
	{
        for (ma_sound *sound : m_Sounds)
        {
			ma_sound_set_spatialization_enabled(sound, enable);
        }
	}

	void AudioSource::SetMinMaxDistance(float minVal, float maxVal)
	{
        for (ma_sound *sound : m_Sounds)
        {
            ma_sound_set_min_distance(sound, minVal);
            ma_sound_set_max_distance(sound, maxVal);
        }
	}

    AudioSource::~AudioSource()
    {
		for (ma_sound *sound : m_Sounds)
		{
			delete sound;
		}
    }

    void AudioSource::LoadSource(const std::string &name, const std::filesystem::path &filepath, bool looping, bool spatializing)
	{
		m_Filepath = filepath;

		ma_sound *sound = new ma_sound;
		AudioEngine::CreateSound(name.c_str(), filepath.generic_string().c_str(), sound);
		m_Sounds.push_back(std::move(sound));
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

	void AudioSource::LoadStreamingSource(const std::string &name, const std::filesystem::path &filepath, bool looping, bool spatializing)
	{
		m_Filepath = filepath;
		m_Name = std::string(name);

		ma_sound *sound = new ma_sound;
		AudioEngine::CreateStreaming(name.c_str(), filepath.generic_string().c_str(), sound);
		m_Sounds.push_back(std::move(sound));
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

    void AudioSource::ActivateOverlapping()
    {
		for (ma_sound *sound : m_Sounds)
		{
			ma_sound_stop(sound);
			ma_sound_seek_to_pcm_frame(sound, 0);
		}

		m_Sounds.clear();
		m_OverlapIndex = 0;
        for (int i = 0; i < m_MaxOverlap; ++i)
        {
			ma_sound *sound = new ma_sound;
            AudioEngine::CreateStreaming(m_Name.c_str(), m_Filepath.generic_string().c_str(), sound);
			m_Sounds.push_back(std::move(sound));
        }

        SetSpatial(m_IsSpatializing);
        if (m_IsSpatializing)
        {
            float defMin = 10.0f;
            float defMax = 20.0f;
            SetMinMaxDistance(defMin, defMax);
        }
		m_OverlappingAllowed = true;
		OGN_CORE_TRACE("[Audio Source] Sound {0} overlapping activated", m_Name);
		PUSH_CONSOLE_INFO("[Audio Source] Sound {0} overlapping activated", m_Name);
    }

    void AudioSource::DeactivateOverlapping()
    {
		m_Sounds.erase(m_Sounds.begin() + 1, m_Sounds.end());
		m_OverlapIndex = 0;
		m_OverlappingAllowed = false;
        OGN_CORE_TRACE("[Audio Source] Sound {0} overlapping deactivated", m_Name);
        PUSH_CONSOLE_INFO("[Audio Source] Sound {0} overlapping deactivated", m_Name);
    }

    std::shared_ptr<AudioSource> AudioSource::Create()
	{
		return std::make_shared<AudioSource>();
	}
}