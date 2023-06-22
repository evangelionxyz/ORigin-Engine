// Copyright (c) 2023 Evangelion Manuhutu | ORigin Engine

#pragma once
#include <string>
#include <fmod.hpp>
#include <fmod_errors.h>

namespace origin {

	struct AudioConfig {
		std::string Name;
		std::string Filepath;
		float MinDistance = 2.0f;
		float MaxDistance = 100.0f;
		bool Looping = false;
		bool Spatial = false;
	};

	class Audio
	{
	public:
		Audio() = default;
		Audio(const AudioConfig& config);
		Audio(const std::string& name, const std::filesystem::path& filepath, bool loop = false, bool spatial = false);
		~Audio();
		
		bool IsPlaying() { return !m_Paused; }
		bool IsPaused() { return m_Paused; }
		bool IsLooping() { return m_Config.Looping; }
		bool IsSpatial() { return m_Config.Spatial; }

		void SetLoop(bool enable);
		void SetGain(float volume);
		void SetName(const std::string& name);
		void SetPosition(const glm::vec3& position);

		void Play();
		void Pause(bool paused);
		void Stop();
		void Release();

		void SetMinDistance(float value);
		void SetMaxDistance(float value);

		float GetMinDistance();
		float GetMaxDistance();

		float GetGain();
		const std::string& GetFilepath() { return m_Config.Filepath; }
		std::string& GetName() { return m_Config.Name; }

		static std::shared_ptr<Audio> Create(const AudioConfig& spec);
		static std::shared_ptr<Audio> Create(const std::string& name, const std::string& filepath, bool loop = false, bool spatial = false);

		bool operator == (const Audio& rhs)
		{
			return (m_Sound == rhs.m_Sound && m_Config.Name == rhs.m_Config.Name);
		}

		bool operator != (const Audio& rhs)
		{
			return (m_Sound != rhs.m_Sound && m_Config.Name != rhs.m_Config.Name);
		}

	private:
		AudioConfig m_Config;
		FMOD_VECTOR m_AudioPosition = {0.0f, 0.0f, 0.0f};

		FMOD::Sound* m_Sound = nullptr;
		FMOD::Channel* m_Channel = nullptr;

		float m_Gain = 1.0f;
		float m_Pitch = 1.0f;
		bool m_Paused = false;
	};

	class AudioEngine
	{
	public:
		static void Init();
		static void Shutdown();

		static FMOD::System* GetSystem();

		static FMOD::Sound* CreateSound(const std::string& name, const std::string& filepath, FMOD_MODE mode);
		static void SystemUpdate();

		static bool AudioStorageInsert(std::shared_ptr<Audio>& audio);
		static bool AudioStorageInsert(const std::string& name, std::shared_ptr<Audio>& audio);
		static bool AudioStorageDelete(std::shared_ptr<Audio>& audio);
		static bool AudioStorageExist(const std::string& name);

		static void SetListener(const glm::vec3& position, const glm::vec3& forward, const glm::vec3& up);

		FMOD_VECTOR m_ListenerPosition = { 0.0f, 0.0f, 0.0f };
		FMOD_VECTOR m_ListenerForward = { 0.0f, 0.0f, -1.0f };;
		FMOD_VECTOR m_ListenerUp = { 0.0f, 1.0f, 0.0f };;

		static std::unordered_map<std::string, std::shared_ptr<Audio>>& GetMap();
	};
	
}

