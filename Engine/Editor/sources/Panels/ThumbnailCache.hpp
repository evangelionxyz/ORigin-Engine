// Copyright (c) Evangelion Manuhutu | ORigin Engine

#pragma once
#include "Origin/Project/Project.h"
#include "Origin/Renderer/Texture.h"

#include <queue>

namespace origin {

    class ThumbnailCache
    {
    public:
        ThumbnailCache() = default;
        ThumbnailCache(const Ref<Project>& project);
        Ref<Texture2D> GetOrCreateThumbnail(const std::filesystem::path& path);

        void OnUpdate();

        void Clear();

    private:
        Ref<Project> m_Project;
        uint64_t m_ThumbnailSize = 128;

        struct ThumbnailImage
        {
            uint64_t Timestamp;
            Ref<Texture2D> Image;
        };

        std::map<std::filesystem::path, ThumbnailImage> m_CachedImages;

        struct ThumbnailInfo 
        {
            std::filesystem::path AbsolutePath;
            std::filesystem::path AssetPath;
            uint64_t Timestamp;
        };

        std::queue<ThumbnailInfo> m_Queue;
        std::filesystem::path m_ThumbnailCachePath;
    };
}

