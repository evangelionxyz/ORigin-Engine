// Copyright (c) Evangelion Manuhutu | ORigin Engine

#include "ThumbnailCache.h"
#include "Origin/Asset/AssetImporter.h"

namespace origin {

    ThumbnailCache::ThumbnailCache(const Ref<Project>& project)
        : m_Project(project)
    {
        m_ThumbnailCachePath = project->GetAssetDirectory() / "Thumbnail.cache";
    }

    Ref<Texture2D> ThumbnailCache::GetOrCreateThumbnail(const std::filesystem::path& asset_path)
    {
        auto absolute_path = m_Project->GetAssetAbsolutePath(asset_path);

        if (!std::filesystem::exists(absolute_path))
            return nullptr;

        std::filesystem::file_time_type lastWriteTime = std::filesystem::last_write_time(absolute_path);
        u32 timestamp = std::chrono::duration_cast<std::chrono::seconds>(lastWriteTime.time_since_epoch()).count();

        if (m_CachedImages.find(asset_path) != m_CachedImages.end())
        {
            auto &cachedImage = m_CachedImages.at(asset_path);
            if (cachedImage.Timestamp == timestamp)
                return cachedImage.Image;
        }

        m_Queue.push({absolute_path, asset_path, timestamp});
        return nullptr;
    }

    void ThumbnailCache::OnUpdate()
    {
        while (!m_Queue.empty())
        {
            const auto &thumbnailInfo = m_Queue.front();

            if (m_CachedImages.find(thumbnailInfo.AssetPath) != m_CachedImages.end())
            {
                auto &cachedImage = m_CachedImages.at(thumbnailInfo.AssetPath);
                if (cachedImage.Timestamp == thumbnailInfo.Timestamp)
                {
                    m_Queue.pop();
                    continue;
                }
            }

            Ref<Texture2D> texture = TextureImporter::LoadTexture2D(thumbnailInfo.AbsolutePath);
            if (!texture)
            {
                m_Queue.pop();
                continue;
            }

            float thumbnailHeight = m_ThumbnailSize * ((float)texture->GetHeight() / (float)texture->GetWidth());
            texture->ChangeSize(m_ThumbnailSize, static_cast<u64>(thumbnailHeight));

            auto &cachedImage = m_CachedImages[thumbnailInfo.AssetPath];
            cachedImage.Timestamp = thumbnailInfo.Timestamp;
            cachedImage.Image = texture;

            m_Queue.pop();
            break;
        }
    }

}