// Copyright (c) Evangelion Manuhutu | ORigin Engine

#pragma once
#include <string>
#include <fmod.hpp>
#include <fmod_errors.h>

#include "Origin\Asset\Asset.h"

namespace origin {

	struct AudioConfig 
	{
		std::string Name;
		float MinDistance = 2.0f;
		float MaxDistance = 100.0f;
		bool Looping = false;
		bool Spatial = false;
	};

	class Audio : public Asset
	{
	public:
		Audio();
		~Audio();
		
		bool IsPlaying() { return !m_Paused; }
		bool IsPaused() { return m_Paused; }
		bool IsLooping() { return m_Config.Looping; }
		bool IsSpatial() { return m_Config.Spatial; }
		bool IsLoaded() { return m_IsLoaded; }

		void SetDopplerLevel(float doppler_level);
		void SetLoop(bool enable);
		void SetGain(float volume);
		void SetPitch(float pitch);
		void SetName(const std::string& name);
		void Set3DAttributes(const glm::vec3& position, const glm::vec3& velocity);
		void SetSpatial(bool enable);

		void LoadSource(const AudioConfig& config);
		void LoadSource(const std::filesystem::path& filepath, const AudioConfig& config);
		void LoadSource(const std::string& name, const std::filesystem::path& filepath, bool loop = false, bool spatial = false);

		void Play();
		void Pause(bool paused);
		void Stop();

		void SetMinDistance(float value);
		void SetMaxDistance(float value);
		float GetMinDistance();
		float GetMaxDistance();

		float GetDopplerLevel() { return m_DopplerLevel; }
		float GetGain() { return m_Gain; }
		float GetPitch() { return m_Pitch; }

		AudioConfig& GetConfig() { return m_Config; }
		const std::filesystem::path& GetFilepath() { return m_Filepath; }
		std::string& GetName() { return m_Config.Name; }

		static AssetType GetStaticType() { return AssetType::Audio; }
		virtual AssetType GetType() const { return GetStaticType(); }

		static std::shared_ptr<Audio> Create();


		void UpdateDopplerEffect(const glm::vec3& listenerPos, const glm::vec3& listenerVelocity);

	private:
		AudioConfig m_Config;
		FMOD_VECTOR m_AudioPosition = {0.0f, 0.0f, 0.0f};
		FMOD_VECTOR m_AudioVelocity = {0.0f, 0.0f, 0.0f};

		FMOD::Sound* m_Sound = nullptr;
		FMOD::Channel* m_Channel = nullptr;

		float m_DopplerLevel = 1.0f;
		float m_Gain = 1.0f;
		float m_Pitch = 1.0f;
		bool m_Paused = false;
		bool m_IsLoaded = false;

		std::filesystem::path m_Filepath;
	};

	class AudioEngine
	{
	public:
		static bool Init();
		static void Shutdown();

		static FMOD::System* GetSystem();

		static FMOD::Sound* CreateSound(const std::string& name, const std::string& filepath, FMOD_MODE mode);
		static void SetMute(bool enable);
		static void SystemUpdate();

		static bool AudioStorageInsert(std::shared_ptr<Audio>& audio);
		static bool AudioStorageInsert(const std::string& name, std::shared_ptr<Audio>& audio);
		static bool AudioStorageDelete(std::shared_ptr<Audio>& audio);
		static bool AudioStorageExist(const std::string& name);

		static std::unordered_map<std::string, std::shared_ptr<Audio>>& GetMap();
	};
	
}

