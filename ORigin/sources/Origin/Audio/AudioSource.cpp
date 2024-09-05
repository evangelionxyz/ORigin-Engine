// Copyright (c) Evangelion Manuhutu | ORigin Engine

#include "pch.h"
#include "AudioSource.h"
#include "Origin/Core/ConsoleManager.h"

namespace origin
{
    void AudioSource::Play() const
    {
        if (ma_sound_is_playing(m_Sounds[0]))
            return;

        if (const ma_result result = ma_sound_start(m_Sounds[0]); result != MA_SUCCESS)
        {
            OGN_CORE_ERROR("[Audio Source] Failed to start sound: {0}", result);
            PUSH_CONSOLE_ERROR("[Audio Source] Failed to start sound: {0}", result);
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

    void AudioSource::PlayLooped() const
    {
		ma_sound_set_looping(m_Sounds[0], MA_TRUE);
		ma_sound_start(m_Sounds[0]);
    }

    void AudioSource::PlayOverlapping()
    {
		if (!m_OverlappingAllowed)
		{
            OGN_CORE_ERROR("[Audio Source] Sound overlapping is not allowed {0}", m_Name);
			PUSH_CONSOLE_ERROR("[Audio Source] Sound overlapping is not allowed {0}", m_Name);
			return;
		}

		if (const ma_result result = ma_sound_start(m_Sounds[m_OverlapIndex]); result != MA_SUCCESS)
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

    void AudioSource::SetVolume(const float volume)
    {
        for (ma_sound *sound : m_Sounds)
        {
            ma_sound_set_volume(sound, volume);
        }
    }

    void AudioSource::SetPitch(const float pitch)
    {
        for (ma_sound *sound : m_Sounds)
        {
            ma_sound_set_pitch(sound, pitch);
        }
    }

    void AudioSource::SetPanning(const float pan)
    {
        for (ma_sound *sound : m_Sounds)
        {
            ma_sound_set_pan(sound, pan);
        }
    }

    void AudioSource::SetPosition(const glm::vec3 &position, const int index, const ma_positioning mode)
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

    bool AudioSource::IsPlaying() const
    {
		return ma_sound_is_playing(m_Sounds[0]) != 0;
	}

	bool AudioSource::IsPaused() const
	{
		return ma_sound_is_playing(m_Sounds[0]) == 0;
	}

	bool AudioSource::IsLooping() const
	{
		return ma_sound_is_looping(m_Sounds[0]);
	}

	bool AudioSource::IsSpatial() const
	{
		return ma_sound_is_spatialization_enabled(m_Sounds[0]);
	}

	float AudioSource::GetVolume() const
	{
		return ma_sound_get_volume(m_Sounds[0]);
	}

	float AudioSource::GetPitch() const
	{
		return ma_sound_get_pitch(m_Sounds[0]);
	}

	float AudioSource::GetMinDistance() const
	{
		return ma_sound_get_min_distance(m_Sounds[0]);
	}

	float AudioSource::GetMaxDistance() const
	{
		return ma_sound_get_max_distance(m_Sounds[0]);
	}

	glm::vec3 AudioSource::GetPosition(const int index) const
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

	void AudioSource::SetSpatial(const bool enable)
	{
        for (ma_sound *sound : m_Sounds)
        {
			ma_sound_set_spatialization_enabled(sound, enable);
        }
	}

	void AudioSource::SetMinMaxDistance(const float minVal, const float maxVal)
	{
        for (ma_sound *sound : m_Sounds)
        {
            ma_sound_set_min_distance(sound, minVal);
            ma_sound_set_max_distance(sound, maxVal);
        }
	}

    AudioSource::~AudioSource()
    {
		for (const ma_sound *sound : m_Sounds)
		{
			delete sound;
		}
    }

    void AudioSource::LoadSource(const std::string &name, const std::filesystem::path &filepath,
    	const bool looping, bool const spatial)
	{
		m_Filepath = filepath;

		auto *sound = new ma_sound;
		AudioEngine::CreateSound(name.c_str(), filepath.generic_string().c_str(), sound);
		m_Sounds.push_back(sound);
		SetLoop(looping);
		SetSpatial(spatial);
		if (spatial)
		{
			constexpr float defMin = 10.0f;
			constexpr float defMax = 20.0f;
			SetMinMaxDistance(defMin, defMax);
		}
		IsLoaded = true;
	}

	void AudioSource::LoadStreamingSource(const std::string &name, const std::filesystem::path &filepath,
		const bool looping, const bool spatial)
	{
		m_Filepath = filepath;
		m_Name = std::string(name);

		auto *sound = new ma_sound;
		AudioEngine::CreateStreaming(name.c_str(), filepath.generic_string().c_str(), sound);
		m_Sounds.push_back(sound);
		SetLoop(looping);
		SetSpatial(spatial);
		if (spatial)
		{
			constexpr float defMin = 10.0f;
			constexpr float defMax = 20.0f;
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
			auto *sound = new ma_sound;
            AudioEngine::CreateStreaming(m_Name.c_str(), m_Filepath.generic_string().c_str(), sound);
			m_Sounds.push_back(sound);
        }

        SetSpatial(m_IsSpatial);
        if (m_IsSpatial)
        {
            constexpr float defMin = 10.0f;
            constexpr float defMax = 20.0f;
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