// Copyright (c) Evangelion Manuhutu | ORigin Engine

#include "pch.h"
#include "Audio.h"
#include <glm\glm.hpp>

namespace origin {

	struct AudioData
	{
		FMOD::System* AudioSystem;
		std::unordered_map<std::string, std::shared_ptr<Audio>> AudioStorage;
		const int MAX_CHANNELS = 32;
	};
	static AudioData s_Data;

	Audio::Audio()
	{
	}

	Audio::~Audio()
	{
		if (m_Channel)
		{
			m_Channel->stop();
			m_Channel = nullptr;
		}

		m_Sound = nullptr;
	}

	void Audio::Play()
	{
		if (!m_Sound)
		{
			OGN_CORE_WARN("AudioSource: NO SOUND CREATED OR INVALID!! Check the filepath");
			return;
		}

		if (m_Channel)
			Stop();

		bool playing;
		m_Channel->isPlaying(&playing);

		if (!playing)
		{
			s_Data.AudioSystem->playSound(m_Sound, nullptr, m_Paused, &m_Channel);
		}
	}

	void Audio::Pause(bool paused)
	{
		m_Paused = paused;
		m_Channel->setPaused(paused);
	}

	void Audio::Stop()
	{
		m_Channel->stop();
	}

	void Audio::SetGain(float volume)
	{
		m_Gain = volume;
		m_Channel->setVolume(m_Gain * 0.01f);
	}

	void Audio::SetPitch(float pitch)
	{
		m_Pitch = pitch;
		m_Channel->setPitch(m_Pitch);
	}

	void Audio::SetLowPassFilter(float value)
	{
		m_LPFilter = value;
		m_Channel->setLowPassGain(value);
	}

	void Audio::SetMinDistance(float value)
	{
		m_Config.MinDistance = value;
		m_Channel->set3DMinMaxDistance(m_Config.MinDistance, m_Config.MaxDistance);
	}

	void Audio::SetMaxDistance(float value)
	{
		m_Config.MaxDistance = value;
		m_Channel->set3DMinMaxDistance(m_Config.MinDistance, m_Config.MaxDistance);
	}

	float Audio::GetMinDistance()
	{
		return m_Config.MinDistance;
	}

	float Audio::GetMaxDistance()
	{
		return m_Config.MaxDistance;
	}

	void Audio::UpdateAudioComponent(const AudioComponent& ac)
	{
		if (m_Channel)
		{
			SetPitch(ac.Pitch);
			SetLowPassFilter(ac.LowPass);
			SetDopplerLevel(ac.DopplerLevel);
			SetMaxDistance(ac.MaxDistance);
			SetMinDistance(ac.MinDistance);
			SetLoop(ac.Looping);
			SetGain(ac.Volume);
		}
	}

	void Audio::SetDopplerLevel(float doppler_level)
	{
		m_DopplerLevel = doppler_level;
		m_Channel->set3DDopplerLevel(m_DopplerLevel);
	}

	void Audio::SetLoop(bool loop)
	{
		m_Config.Looping = loop;
		m_Sound->setMode(loop ? FMOD_LOOP_NORMAL : FMOD_LOOP_OFF);
	}

	void Audio::SetName(const std::string& name)
	{
		m_Config.Name = name;
	}

	void Audio::Set3DAttributes(const glm::vec3& position, const glm::vec3& velocity)
	{
		// Inversing position by default
		m_AudioPosition = { -position.x, -position.y, -position.z };
		m_AudioVelocity = { -velocity.x, -velocity.y, -velocity.z };

		m_Channel->set3DAttributes(&m_AudioPosition, &m_AudioVelocity);
	}

	void Audio::SetSpatial(bool enable)
	{
		// Stop the audio to get the effect
		Audio::Stop();

		if (enable)
			OGN_CORE_WARN("AudioSource {0}: SPATIALIZATION ON", m_Config.Name);
		else
			OGN_CORE_WARN("AudioSource {0}: SPATIALIZATION OFF", m_Config.Name);

		m_Config.Spatial = enable;
		m_Sound->setMode(enable ? FMOD_3D : FMOD_2D);
	}

	void Audio::LoadSource(const AudioConfig& config)
	{
		LoadSource(config.Name, m_Filepath, config.Looping, config.Spatial);
	}

	void Audio::LoadSource(const std::filesystem::path& filepath, const AudioConfig& config)
	{
		LoadSource(config.Name, filepath, config.Looping, config.Spatial);
	}

	void Audio::LoadSource(const std::string& name, const std::filesystem::path& filepath, bool loop, bool spatial)
	{
		m_Filepath = filepath;

		m_Config.Name = name;
		m_Config.Looping = loop;
		m_Config.Spatial = spatial;

		m_Sound = AudioEngine::CreateSound(name, m_Filepath.string().c_str(), spatial ? FMOD_3D : FMOD_2D);
		m_Sound->setMode(m_Config.Looping ? FMOD_LOOP_NORMAL : FMOD_LOOP_OFF);

		if (m_Config.Spatial)
		{
			s_Data.AudioSystem->set3DSettings(1.0, 1.0f, 1.0f);
			m_Channel->set3DAttributes(&m_AudioPosition, nullptr);
			m_Channel->set3DMinMaxDistance(m_Config.MinDistance, m_Config.MaxDistance);
			m_Channel->set3DDopplerLevel(m_DopplerLevel);
		}

		m_IsLoaded = true;
	}

	std::shared_ptr<Audio> Audio::Create()
	{
		return std::make_shared<Audio>();
	}

	void Audio::UpdateDopplerEffect(const glm::vec3& listenerPos, const glm::vec3& listenerVelocity)
	{
		glm::vec3 audioPosition = glm::vec3(m_AudioPosition.x, m_AudioPosition.y, m_AudioPosition.z);
		glm::vec3 audioVelocity = glm::vec3(m_AudioVelocity.x, m_AudioVelocity.y, m_AudioVelocity.z);

		glm::vec3 relativeVelocity = listenerVelocity - audioVelocity;
		float distance = glm::length(listenerPos - audioPosition);
		float dopplerPitch = 1.0f;

		if (distance > 0.001f) {
			float speedOfSound = 343.0f;

			float dopplerShift = speedOfSound / (speedOfSound - glm::dot(relativeVelocity, glm::normalize(listenerPos - audioPosition)));
			dopplerPitch = dopplerShift;
		}

		m_Channel->setPitch(dopplerPitch);
	}

	//////////////////////////////////////////////
	//////////////// Audio Engine ////////////////
	//////////////////////////////////////////////

	bool AudioEngine::Init()
	{
		FMOD::System_Create(&s_Data.AudioSystem);
		s_Data.AudioSystem->init(s_Data.MAX_CHANNELS, FMOD_INIT_NORMAL | FMOD_INIT_CHANNEL_LOWPASS, 0);

		return true;
	}

	void AudioEngine::Shutdown()
	{
		s_Data.AudioSystem->close();
		s_Data.AudioSystem->release();

		OGN_CORE_WARN("AudioEngine: Shutdown");
	}

	FMOD::System* AudioEngine::GetSystem()
	{
		return s_Data.AudioSystem;
	}

	FMOD::Sound* AudioEngine::CreateSound(const std::string& name, const std::string& filepath, FMOD_MODE mode)
	{
		FMOD::Sound* sound;
		s_Data.AudioSystem->createSound(filepath.c_str(),
			mode, nullptr, &sound);


		return sound;
	}

	void AudioEngine::SetMute(bool enable)
	{
		FMOD::ChannelGroup* channelControl;
		s_Data.AudioSystem->getMasterChannelGroup(&channelControl);

		if (enable)
			OGN_CORE_ERROR("AudioEngine: MASTER CHANNEL MUTED!");

		channelControl->setMute(enable);
	}

	void AudioEngine::SystemUpdate()
	{
		s_Data.AudioSystem->update();
	}

	bool AudioEngine::AudioStorageInsert(std::shared_ptr<Audio>& audio)
	{
		return AudioStorageInsert(audio->GetName(), audio);
	}

	bool AudioEngine::AudioStorageInsert(const std::string& name, std::shared_ptr<Audio>& audio)
	{
		if (AudioStorageExist(name))
		{
			OGN_CORE_ERROR("AudioStorage: {} Already Exist", name);
			return false;
		}

		s_Data.AudioStorage.insert(std::make_pair(name, audio));
		return true;
	}

	bool AudioEngine::AudioStorageDelete(std::shared_ptr<Audio>& audio)
	{
		std::string name = audio->GetName();
		if (!AudioStorageExist(name))
		{
			OGN_CORE_ERROR("AudioStorage: {} Doesn't Exist", name);
			return false;
		};

		s_Data.AudioStorage.erase(name);
		return true;
	}
	bool AudioEngine::AudioStorageExist(const std::string& name)
	{
		auto& it = s_Data.AudioStorage.find(name);
		return it != s_Data.AudioStorage.end();
	}

	std::unordered_map<std::string, std::shared_ptr<Audio>>& AudioEngine::GetMap()
	{
		return s_Data.AudioStorage;
	}
}