// Copyright (c) Evangelion Manuhutu | ORigin Engine

#include "pch.h"
#include "Audio.h"
#include <glm\glm.hpp>

namespace origin {

#define FMOD_CHECK(result) if (result != FMOD_OK) {/*OGN_CORE_ERROR("FMOD AUDIO ERROR: {}", FMOD_ErrorString(result));*/}

	struct AudioData
	{
		FMOD_RESULT Result;
		FMOD::System* AudioSystem;

		std::unordered_map<std::string, std::shared_ptr<Audio>> AudioStorage;
		const int MAX_CHANNELS = 32;
	};

	static AudioData s_AudioEngineData;

	Audio::Audio()
	{
	}

	Audio::~Audio()
	{
		Release();
	}

	void Audio::Play()
	{
		if (m_Sound == nullptr)
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
			//OGN_CORE_WARN("AudioSource: Playing {}", m_Config.Name);

			s_AudioEngineData.Result = AudioEngine::GetSystem()->playSound(m_Sound, nullptr, m_Paused, &m_Channel);
			FMOD_CHECK(s_AudioEngineData.Result);
		}
	}

	void Audio::Pause(bool paused)
	{
		m_Paused = paused;
		s_AudioEngineData.Result = m_Channel->setPaused(paused);
		FMOD_CHECK(s_AudioEngineData.Result);
	}

	void Audio::Stop()
	{
		s_AudioEngineData.Result = m_Channel->stop();
		FMOD_CHECK(s_AudioEngineData.Result);
	}

	void Audio::SetGain(float volume)
	{
		m_Gain = volume;
		s_AudioEngineData.Result = m_Channel->setVolume(m_Gain);
		FMOD_CHECK(s_AudioEngineData.Result);
	}

	float Audio::GetGain()
	{
		return m_Gain;
	}

	void Audio::SetMinDistance(float value)
	{
		m_Config.MinDistance = value;
		s_AudioEngineData.Result = m_Channel->set3DMinMaxDistance(m_Config.MinDistance, m_Config.MaxDistance);
		FMOD_CHECK(s_AudioEngineData.Result);
	}

	void Audio::SetMaxDistance(float value)
	{
		m_Config.MaxDistance = value;
		s_AudioEngineData.Result = m_Channel->set3DMinMaxDistance(m_Config.MinDistance, m_Config.MaxDistance);
		FMOD_CHECK(s_AudioEngineData.Result);
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
		OGN_CORE_WARN("AudioSource: Releasing {}", m_Config.Name);

		s_AudioEngineData.Result = m_Channel->stop();
		FMOD_CHECK(s_AudioEngineData.Result);
		m_Channel = nullptr;
		m_Sound = nullptr;
	}

	void Audio::SetLoop(bool loop)
	{
		m_Config.Looping = loop;

		s_AudioEngineData.Result = m_Sound->setMode(loop ? FMOD_LOOP_NORMAL : FMOD_LOOP_OFF);
		FMOD_CHECK(s_AudioEngineData.Result);
	}

	void Audio::SetName(const std::string& name)
	{
		m_Config.Name = name;
	}

	void Audio::SetPosition(const glm::vec3& position)
	{
		// Inversing position by default
		m_AudioPosition = { -position.x, -position.y, -position.z };

		FMOD_VECTOR velocity = { 1.0f, 1.0f, 1.0f };
		s_AudioEngineData.Result = m_Channel->set3DAttributes(&m_AudioPosition, &velocity);
		FMOD_CHECK(s_AudioEngineData.Result);
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
		s_AudioEngineData.Result = m_Sound->setMode(enable ? FMOD_3D : FMOD_2D);
	}

	void Audio::LoadSource(const AudioConfig& config)
	{
		LoadSource(config.Name, config.Filepath, config.Looping, config.Spatial);
	}

	void Audio::LoadSource(const std::string& name, const std::filesystem::path& filepath, bool loop, bool spatial)
	{
		m_Config.Name = name;
		m_Config.Filepath = filepath.string();
		m_Config.Looping = loop;
		m_Config.Spatial = spatial;

		m_Sound = AudioEngine::CreateSound(name, m_Config.Filepath.c_str(), spatial ? FMOD_3D : FMOD_2D);
		FMOD_CHECK(s_AudioEngineData.Result);

		s_AudioEngineData.Result = m_Sound->setMode(m_Config.Looping ? FMOD_LOOP_NORMAL : FMOD_LOOP_OFF);
		FMOD_CHECK(s_AudioEngineData.Result)

		if (m_Config.Spatial)
		{
			s_AudioEngineData.Result = s_AudioEngineData.AudioSystem->set3DSettings(1.0, 1.0f, 1.0f);
			FMOD_CHECK(s_AudioEngineData.Result);

			s_AudioEngineData.Result = m_Channel->set3DAttributes(&m_AudioPosition, nullptr);
			FMOD_CHECK(s_AudioEngineData.Result);

			s_AudioEngineData.Result = m_Channel->set3DMinMaxDistance(m_Config.MinDistance, m_Config.MaxDistance);
			FMOD_CHECK(s_AudioEngineData.Result);
		}

		m_IsLoaded = true;
	}

	std::shared_ptr<Audio> Audio::Create()
	{
		return std::make_shared<Audio>();
	}



	//////////////////////////////////////////////
	//////////////// Audio Engine ////////////////
	//////////////////////////////////////////////

	bool AudioEngine::Init()
	{
		s_AudioEngineData.Result = FMOD::System_Create(&s_AudioEngineData.AudioSystem);
		FMOD_CHECK(s_AudioEngineData.Result);
		s_AudioEngineData.Result = s_AudioEngineData.AudioSystem->init(s_AudioEngineData.MAX_CHANNELS, FMOD_INIT_NORMAL, 0);
		FMOD_CHECK(s_AudioEngineData.Result);

		return true;
	}

	void AudioEngine::Shutdown()
	{
		s_AudioEngineData.Result = s_AudioEngineData.AudioSystem->close();
		s_AudioEngineData.Result = s_AudioEngineData.AudioSystem->release();

		OGN_CORE_WARN("AudioEngine: Shutdown");
	}

	FMOD::System* AudioEngine::GetSystem()
	{
		return s_AudioEngineData.AudioSystem;
	}

	FMOD::Sound* AudioEngine::CreateSound(const std::string& name, const std::string& filepath, FMOD_MODE mode)
	{
		FMOD::Sound* sound;
		s_AudioEngineData.Result = s_AudioEngineData.AudioSystem->createSound(filepath.c_str(),
			mode, nullptr, &sound);

		FMOD_CHECK(s_AudioEngineData.Result);

		return sound;
	}

	void AudioEngine::SetMute(bool enable)
	{
		FMOD::ChannelGroup* channelControl;
		s_AudioEngineData.AudioSystem->getMasterChannelGroup(&channelControl);

		if (enable)
			OGN_CORE_ERROR("AudioEngine: MASTER CHANNEL MUTED!");

		s_AudioEngineData.Result = channelControl->setMute(enable);
		FMOD_CHECK(s_AudioEngineData.Result);
	}

	void AudioEngine::SystemUpdate()
	{
		s_AudioEngineData.Result = s_AudioEngineData.AudioSystem->update();
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

		s_AudioEngineData.AudioStorage.insert(std::make_pair(name, audio));
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

		s_AudioEngineData.AudioStorage.erase(name);
		return true;
	}
	bool AudioEngine::AudioStorageExist(const std::string& name)
	{
		auto& it = s_AudioEngineData.AudioStorage.find(name);
		return it != s_AudioEngineData.AudioStorage.end();
	}

	std::unordered_map<std::string, std::shared_ptr<Audio>>& AudioEngine::GetMap()
	{
		return s_AudioEngineData.AudioStorage;
	}
}