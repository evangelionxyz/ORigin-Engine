// Copyright (c) Evangelion Manuhutu | ORigin Engine

#include "ThumbnailCache.h"
#include "Origin\Asset\AssetImporter.h"

#pragma warning(disable : OGN_DISABLED_WARNINGS)

namespace origin {

	ThumbnailCache::ThumbnailCache(const std::shared_ptr<Project>& project)
		: m_Project(project)
	{
		m_ThumbnailCachePath = project->GetAssetDirectory() / "Thumbnail.cache";
	}

	std::shared_ptr<Texture2D> ThumbnailCache::GetOrCreateThumbnail(const std::filesystem::path& assetPath)
	{
		auto absolutePath = m_Project->GetAssetAbsolutePath(assetPath);
		std::filesystem::file_time_type lastWriteTime = std::filesystem::last_write_time(absolutePath);
		uint64_t timestamp = std::chrono::duration_cast<std::chrono::seconds>(lastWriteTime.time_since_epoch()).count();

		if (m_CachedImages.find(assetPath) != m_CachedImages.end())
		{
			auto &cachedImage = m_CachedImages.at(assetPath);
			if (cachedImage.Timestamp == timestamp)
				return cachedImage.Image;
		}

		m_Queue.push({absolutePath, assetPath, timestamp});
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

			std::shared_ptr<Texture2D> texture = TextureImporter::LoadTexture2D(thumbnailInfo.AbsolutePath);
			if (!texture)
			{
				m_Queue.pop();
				continue;
			}

			float thumbnailHeight = m_ThumbnailSize * ((float)texture->GetHeight() / (float)texture->GetWidth());
			texture->ChangeSize(m_ThumbnailSize, thumbnailHeight);

			auto &cachedImage = m_CachedImages[thumbnailInfo.AssetPath];
			cachedImage.Timestamp = thumbnailInfo.Timestamp;
			cachedImage.Image = texture;

			m_Queue.pop();
			break;
		}
	}

}