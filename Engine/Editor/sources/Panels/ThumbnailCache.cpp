// Copyright (c) Evangelion Manuhutu | ORigin Engine

#include "ThumbnailCache.hpp"
#include "Origin/Asset/AssetImporter.h"

namespace origin {

    ThumbnailCache::ThumbnailCache(const Ref<Project>& project)
        : m_Project(project)
    {
        m_ThumbnailCachePath = project->GetAssetDirectory() / "Thumbnail.cache";
    }

    Ref<Texture2D> ThumbnailCache::GetOrCreateThumbnail(const std::filesystem::path& asset_path)
    {
        const auto absolute_path = m_Project->GetAssetAbsolutePath(asset_path);

        if (!exists(absolute_path))
            return nullptr;

        std::filesystem::file_time_type last_write_time = std::filesystem::last_write_time(absolute_path);
        const u32 timestamp = std::chrono::duration_cast<std::chrono::seconds>(last_write_time.time_since_epoch()).count();

        if (m_CachedImages.contains(asset_path))
        {
            const auto &[Timestamp, Image] = m_CachedImages.at(asset_path);
            if (Timestamp == timestamp)
            {
                return Image;
            }
        }

        m_Queue.push({absolute_path, asset_path, timestamp});
        return nullptr;
    }

    void ThumbnailCache::OnUpdate()
    {
        while (!m_Queue.empty())
        {
            const auto &thumbnail_info = m_Queue.front();
            if (m_CachedImages.contains(thumbnail_info.AssetPath))
            {
                const auto & [Timestamp, Image] = m_CachedImages.at(thumbnail_info.AssetPath);
                if (Timestamp == thumbnail_info.Timestamp)
                {
                    m_Queue.pop();
                    continue;
                }
            }

            Ref<Texture2D> texture = TextureImporter::LoadTexture2D(thumbnail_info.AbsolutePath);
            if (!texture)
            {
                m_Queue.pop();
                continue;
            }

            const f32 thumbnail_height = m_ThumbnailSize * (static_cast<f32>(texture->GetHeight()) / static_cast<f32>(texture->GetWidth()));
            texture->ChangeSize(m_ThumbnailSize, thumbnail_height);

            auto &cached_image = m_CachedImages[thumbnail_info.AssetPath];
            cached_image.Timestamp = thumbnail_info.Timestamp;
            cached_image.Image = texture;

            m_Queue.pop();
            break;
        }
    }

    void ThumbnailCache::Clear()
    {
        m_CachedImages.clear();
    }

}
