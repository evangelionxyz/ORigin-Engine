#pragma once
#include "AudioEngine.h"

namespace origin
{
	class AudioSource : public Asset
	{
	public:
		AudioSource() = default;

		void LoadSource(const char* name, const std::filesystem::path &filepath, bool looping = false, bool spatializing = false);
		void LoadStreamingSource(const char *name, const std::filesystem::path &filepath, bool looping = false, bool spatializing = false);

		void SetLoop(bool enable);
		void SetVolume(float value);
		void SetPitch(float value);
		void SetPaning(float pan);
		void SetPosition(const glm::vec3 &position, ma_positioning mode = ma_positioning_absolute);
		void SetName(const char* name);
		void SetSpatial(bool enable);
		void SetMinMaxDistance(float minVal, float maxVal);

		void Play();
		void Pause();
		void Stop();

		bool IsPlaying();
		bool IsPaused();
		bool IsLooping();
		bool IsSpatial();
		float GetVolume();
		float GetPitch();
		float GetMinDistance();
		float GetMaxDistance();
		bool IsLoaded = false;

		const glm::vec3 GetPosition() const;

		const char *GetName() { return m_Name; }
		const std::filesystem::path &GetFilepath() { return m_Filepath; }

		static AssetType GetStaticType() { return AssetType::Audio; }
		virtual AssetType GetType() const { return GetStaticType(); }

		static std::shared_ptr<AudioSource> Create();
	private:
		std::unique_ptr<ma_sound> m_Sound;
		const char* m_Name;
		const char* m_Filepath;
	};

}
