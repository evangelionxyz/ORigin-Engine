// Copyright (c) 2022-present Evangelion Manuhutu | ORigin Engine

#ifndef AUDIO_SOURCE_H
#define AUDIO_SOURCE_H

#include "AudioEngine.h"

namespace origin
{
    class Asset;
    class AudioSource : public Asset
    {
    public:
        AudioSource() = default;
        ~AudioSource() override;

        void LoadSource(const std::string &name, const std::filesystem::path &filepath, bool looping = false, bool spatial = false);
        void LoadStreamingSource(const std::string &name, const std::filesystem::path &filepath, bool looping = false, bool spatial = false);
        void ActivateOverlapping();
        void DeactivateOverlapping();

        void SetLoop(bool enable);
        void SetVolume(float value);
        void SetPitch(float value);
        void SetPanning(float pan);
        void SetPosition(const glm::vec3 &position,int index = -1, ma_positioning mode = ma_positioning_absolute);
        void SetName(const char* name);
        void SetSpatial(bool enable);
        void SetMinMaxDistance(float minVal, float maxVal);

        void Play() const;
        void Pause();
        void Stop();
        void PlayLooped() const;
        void PlayOverlapping();

        [[nodiscard]] bool IsPlaying() const;
        [[nodiscard]] bool IsPaused() const;
        [[nodiscard]] bool IsLooping() const;
        [[nodiscard]] bool IsSpatial() const;
        [[nodiscard]] float GetVolume() const;
        [[nodiscard]] float GetPitch() const;
        [[nodiscard]] float GetMinDistance() const;
        [[nodiscard]] float GetMaxDistance() const;

        bool IsLoaded = false;

        [[nodiscard]] glm::vec3 GetPosition(int index = 0) const;

        const std::string &GetName() { return m_Name; }
        std::filesystem::path GetFilepath() { return m_Filepath; }

        static AssetType GetStaticType() { return AssetType::Audio; }
        [[nodiscard]] AssetType GetType() const override { return GetStaticType(); }

        static std::shared_ptr<AudioSource> Create();
    private:
        std::filesystem::path m_Filepath;
        std::string m_Name;
        std::vector<ma_sound*> m_Sounds;
        int m_OverlapIndex = 0;
        int m_MaxOverlap = 50;
        bool m_IsSpatial = false;
        bool m_OverlappingAllowed = false;
    };

}

#endif