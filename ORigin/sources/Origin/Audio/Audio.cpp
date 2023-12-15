// Copyright (c) Evangelion Manuhutu | ORigin Engine

#include "pch.h"
#include "Audio.h"

#include <glm\glm.hpp>

namespace origin {

#define FMOD_CHECK(result) if (result != FMOD_OK) {\
/*OGN_CORE_ERROR("FMOD AUDIO ERROR: {}, {}", __LINE__, FMOD_ErrorString(result));*/}

	struct AudioData
	{
		FMOD_RESULT Result;
		FMOD::System* AudioSystem;
		FMOD::Channel* Channel;
		FMOD::ChannelControl* ChannelControl;

		std::unordered_map<std::string, std::shared_ptr<Audio>> AudioStorage;
		const int MAX_CHANNELS = 32;
	};

	static AudioData s_Data;

	Audio::Audio()
	{
	}

	Audio::~Audio()
	{
		s_Data.Channel->stop();

		if (s_Data.Channel)
			s_Data.Channel = nullptr;

		m_Sound = nullptr;
	}

	void Audio::Play()
	{
		if (m_Sound == nullptr)
		{
			OGN_CORE_WARN("AudioSource: NO SOUND CREATED OR INVALID!! Check the filepath");
			return;
		}

		if (s_Data.Channel)
			Stop();

		bool playing;
		s_Data.Channel->isPlaying(&playing);

		if (!playing)
		{
			//OGN_CORE_WARN("AudioSource: Playing {}", m_Config.Name);
			s_Data.Result = AudioEngine::GetSystem()->playSound(m_Sound, nullptr, m_Paused, &s_Data.Channel);
			FMOD_CHECK(s_Data.Result);
		}
	}

	void Audio::Pause(bool paused)
	{
		m_Paused = paused;
		s_Data.Result = s_Data.Channel->setPaused(paused);
		FMOD_CHECK(s_Data.Result);
	}

	void Audio::Stop()
	{
		s_Data.Result = s_Data.Channel->stop();
		FMOD_CHECK(s_Data.Result);
	}

	void Audio::SetGain(float volume)
	{
		m_Gain = volume;
		s_Data.Result = s_Data.Channel->setVolume(m_Gain * 0.01f);
		FMOD_CHECK(s_Data.Result);
	}

	void Audio::SetPitch(float pitch)
	{
		m_Pitch = pitch;
		s_Data.Result = s_Data.Channel->setPitch(m_Pitch);
		FMOD_CHECK(s_Data.Result);
	}

	void Audio::SetLowPassFilter(float value)
	{
		m_LPFilter = value;
		s_Data.Result = s_Data.Channel->setLowPassGain(value);
		FMOD_CHECK(s_Data.Result);
	}

	void Audio::SetMinDistance(float value)
	{
		m_Config.MinDistance = value;
		s_Data.Result = s_Data.Channel->set3DMinMaxDistance(m_Config.MinDistance, m_Config.MaxDistance);
		FMOD_CHECK(s_Data.Result);
	}

	void Audio::SetMaxDistance(float value)
	{
		m_Config.MaxDistance = value;
		s_Data.Result = s_Data.Channel->set3DMinMaxDistance(m_Config.MinDistance, m_Config.MaxDistance);
		FMOD_CHECK(s_Data.Result);
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
		if (s_Data.Channel)
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
		s_Data.Result = s_Data.Channel->set3DDopplerLevel(m_DopplerLevel);
		FMOD_CHECK(s_Data.Result);
	}

	void Audio::SetLoop(bool loop)
	{
		m_Config.Looping = loop;
		s_Data.Result = m_Sound->setMode(loop ? FMOD_LOOP_NORMAL : FMOD_LOOP_OFF);
		FMOD_CHECK(s_Data.Result);
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

		s_Data.Result = s_Data.Channel->set3DAttributes(&m_AudioPosition, &m_AudioVelocity);
		FMOD_CHECK(s_Data.Result);
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
		s_Data.Result = m_Sound->setMode(enable ? FMOD_3D : FMOD_2D);
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
		FMOD_CHECK(s_Data.Result);

		s_Data.Result = m_Sound->setMode(m_Config.Looping ? FMOD_LOOP_NORMAL : FMOD_LOOP_OFF);
		
		FMOD_CHECK(s_Data.Result)

		if (m_Config.Spatial)
		{
			s_Data.Result = s_Data.AudioSystem->set3DSettings(1.0, 1.0f, 1.0f);
			FMOD_CHECK(s_Data.Result);

			s_Data.Result = s_Data.Channel->set3DAttributes(&m_AudioPosition, nullptr);
			FMOD_CHECK(s_Data.Result);

			s_Data.Result = s_Data.Channel->set3DMinMaxDistance(m_Config.MinDistance, m_Config.MaxDistance);
			FMOD_CHECK(s_Data.Result);

			s_Data.Result = s_Data.Channel->set3DDopplerLevel(m_DopplerLevel);
			FMOD_CHECK(s_Data.Result);
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

		s_Data.Channel->setPitch(dopplerPitch);
	}

	//////////////////////////////////////////////
	//////////////// Audio Engine ////////////////
	//////////////////////////////////////////////

	bool AudioEngine::Init()
	{
		s_Data.Result = FMOD::System_Create(&s_Data.AudioSystem);
		FMOD_CHECK(s_Data.Result);
		s_Data.Result = s_Data.AudioSystem->init(s_Data.MAX_CHANNELS, FMOD_INIT_NORMAL | FMOD_INIT_CHANNEL_LOWPASS, 0);
		FMOD_CHECK(s_Data.Result);

		return true;
	}

	void AudioEngine::Shutdown()
	{
		s_Data.Result = s_Data.AudioSystem->close();
		s_Data.Result = s_Data.AudioSystem->release();

		OGN_CORE_WARN("AudioEngine: Shutdown");
	}

	FMOD::System* AudioEngine::GetSystem()
	{
		return s_Data.AudioSystem;
	}

	FMOD::Sound* AudioEngine::CreateSound(const std::string& name, const std::string& filepath, FMOD_MODE mode)
	{
		FMOD::Sound* sound;
		s_Data.Result = s_Data.AudioSystem->createSound(filepath.c_str(),
			mode, nullptr, &sound);

		FMOD_CHECK(s_Data.Result);

		return sound;
	}

	void AudioEngine::SetMute(bool enable)
	{
		FMOD::ChannelGroup* channelControl;
		s_Data.AudioSystem->getMasterChannelGroup(&channelControl);

		if (enable)
			OGN_CORE_ERROR("AudioEngine: MASTER CHANNEL MUTED!");

		s_Data.Result = channelControl->setMute(enable);
		FMOD_CHECK(s_Data.Result);
	}

	void AudioEngine::SystemUpdate()
	{
		s_Data.Result = s_Data.AudioSystem->update();
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