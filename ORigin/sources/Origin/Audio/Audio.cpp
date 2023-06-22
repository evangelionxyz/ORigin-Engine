// Copyright (c) 2023 Evangelion Manuhutu | ORigin Engine

#include "pch.h"
#include "Audio.h"
#include "glm\glm.hpp"

namespace origin {

#define FMOD_CHECK(result) if (result != FMOD_OK) {\
	/*OGN_CORE_ERROR("FMOD AUDIO ERROR: {}", FMOD_ErrorString(result))*/;}

	struct AudioData
	{
		FMOD_RESULT Result;
		FMOD::System* AudioSystem;
		std::unordered_map<std::string, std::shared_ptr<Audio>> AudioStorage;
		const int MAX_CHANNELS = 32;
	};

	static AudioData s_Data;

	Audio::Audio(const AudioConfig& config)
		: m_Config(config)
	{
		m_Sound = AudioEngine::CreateSound(config.Name, config.Filepath.c_str(), config.Spatial ? FMOD_3D : FMOD_2D);
		FMOD_CHECK(s_Data.Result);

		s_Data.Result = m_Sound->setMode(config.Looping ? FMOD_LOOP_NORMAL : FMOD_LOOP_OFF);
		FMOD_CHECK(s_Data.Result)

		if (m_Config.Spatial)
		{
			s_Data.Result = s_Data.AudioSystem->set3DSettings(1.0, 1.0f, 1.0f);
			FMOD_CHECK(s_Data.Result);

			s_Data.Result = m_Channel->set3DAttributes(&m_AudioPosition, nullptr);
			FMOD_CHECK(s_Data.Result);

			s_Data.Result = m_Channel->set3DMinMaxDistance(m_Config.MinDistance, m_Config.MaxDistance);
			FMOD_CHECK(s_Data.Result);
		}
	}

	Audio::Audio(const std::string& name, const std::filesystem::path& filepath, bool loop, bool spatial)
	{
		m_Config.Name = name;
		m_Config.Filepath = filepath.string();
		m_Config.Looping = loop;
		m_Config.Spatial = spatial;

		m_Sound = AudioEngine::CreateSound(name, m_Config.Filepath.c_str(), FMOD_3D);
		FMOD_CHECK(s_Data.Result);

		s_Data.Result = m_Sound->setMode(m_Config.Looping ? FMOD_LOOP_NORMAL : FMOD_LOOP_OFF);
		FMOD_CHECK(s_Data.Result)

		if (m_Config.Spatial)
		{
			s_Data.Result = s_Data.AudioSystem->set3DSettings(1.0, 1.0f, 1.0f);
			FMOD_CHECK(s_Data.Result);

			s_Data.Result = m_Channel->set3DAttributes(&m_AudioPosition, nullptr);
			FMOD_CHECK(s_Data.Result);

			s_Data.Result = m_Channel->set3DMinMaxDistance(m_Config.MinDistance, m_Config.MaxDistance);
			FMOD_CHECK(s_Data.Result);
		}
	}

	Audio::~Audio()
	{
		Release();
	}

	void Audio::Play()
	{
		if (!AudioEngine::GetSystem() || !m_Sound)
			return;

		if (m_Channel)
			s_Data.Result = m_Channel->stop();

		bool playing;
		m_Channel->isPlaying(&playing);
		if (!playing)
		{
			s_Data.Result = AudioEngine::GetSystem()->playSound(m_Sound, nullptr, m_Paused, &m_Channel);
			FMOD_CHECK(s_Data.Result);
		}
	}

	void Audio::Pause(bool paused)
	{
		m_Paused = paused;
		s_Data.Result = m_Channel->setPaused(paused);
		FMOD_CHECK(s_Data.Result);
	}

	void Audio::Stop()
	{
		s_Data.Result = m_Channel->stop();
		FMOD_CHECK(s_Data.Result);
	}

	void Audio::SetGain(float volume)
	{
		m_Gain = volume;
		s_Data.Result = m_Channel->setVolume(m_Gain);
		FMOD_CHECK(s_Data.Result);
	}

	float Audio::GetGain()
	{
		return m_Gain;
	}

	void Audio::SetMinDistance(float value)
	{
		m_Config.MinDistance = value;
		s_Data.Result = m_Channel->set3DMinMaxDistance(m_Config.MinDistance, m_Config.MaxDistance);
		FMOD_CHECK(s_Data.Result);
	}

	void Audio::SetMaxDistance(float value)
	{
		m_Config.MaxDistance = value;
		s_Data.Result = m_Channel->set3DMinMaxDistance(m_Config.MinDistance, m_Config.MaxDistance);
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

	void Audio::Release()
	{
		OGN_CORE_WARN("Audio Releasing: {}", m_Config.Name);

		s_Data.Result = m_Channel->stop();
		FMOD_CHECK(s_Data.Result);
		m_Channel = nullptr;
		m_Sound = nullptr;
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

	void Audio::SetPosition(const glm::vec3& position)
	{
		m_AudioPosition = { position.x, position.y, position.z };

		s_Data.Result = m_Channel->set3DAttributes(&m_AudioPosition, nullptr);
		FMOD_CHECK(s_Data.Result);
	}

	std::shared_ptr<Audio> Audio::Create(const AudioConfig& spec)
	{
		return std::make_shared<Audio>(spec);
	}

	std::shared_ptr<Audio> Audio::Create(const std::string& name, const std::string& filepath, bool loop, bool spatial)
	{
		return std::make_shared<Audio>(name, filepath, loop, spatial);
	}

	//////////////////////////////////////////////
	//////////////// Audio Engine ////////////////
	//////////////////////////////////////////////

	void AudioEngine::Init()
	{
		s_Data.Result = FMOD::System_Create(&s_Data.AudioSystem);
		FMOD_CHECK(s_Data.Result);
		s_Data.Result = s_Data.AudioSystem->init(s_Data.MAX_CHANNELS, FMOD_INIT_NORMAL, 0);
		FMOD_CHECK(s_Data.Result);
	}

	void AudioEngine::Shutdown()
	{
		s_Data.Result = s_Data.AudioSystem->close();
		s_Data.Result = s_Data.AudioSystem->release();

		OGN_CORE_WARN("Audio Engine Shutdown");
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

	void AudioEngine::SystemUpdate()
	{
		s_Data.Result = s_Data.AudioSystem->update();
	}

	void AudioEngine::SetListener(const glm::vec3& position, const glm::vec3& forward, const glm::vec3& up)
	{
		// Set Global Listener
		FMOD_VECTOR listenerPos = { -position.x, -position.y, -position.z };
		FMOD_VECTOR listenerForward = { forward.x, forward.y, forward.z };
		FMOD_VECTOR listenerUp = { up.x, up.y, up.z };

		s_Data.Result = s_Data.AudioSystem->set3DListenerAttributes(0, &listenerPos, nullptr, &listenerForward, &listenerUp);
		FMOD_CHECK(s_Data.Result);
	}

	bool AudioEngine::AudioStorageInsert(std::shared_ptr<Audio>& audio)
	{
		return AudioStorageInsert(audio->GetName(), audio);
	}

	bool AudioEngine::AudioStorageInsert(const std::string& name, std::shared_ptr<Audio>& audio)
	{
		if (AudioStorageExist(name))
		{
			OGN_CORE_ERROR("Audio '{}' Already Exist", name);
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
			OGN_CORE_ERROR("Audio '{}' Doesn't Exist", name);
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