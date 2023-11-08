#include "ThumbnailCache.h"

#include "Origin\Asset\TextureImporter.h"

namespace origin {

	ThumbnailCache::ThumbnailCache(std::shared_ptr<Project> project)
		: m_Project(project)
	{
		m_ThumbnailCachePath = project->GetAssetDirectory() / "Thumbnail.cache";
	}

	std::shared_ptr<Texture2D> ThumbnailCache::GetOrCreateThumbnail(const std::filesystem::path assetPath)
	{
		// 1. Read file
		// 2. Compare hashed timestamp with existing cached image (in memory first, then from cache file)
		// 3. If equal, return associated thumbnail, otherwise load asset from disk
		// 4. If generated new thumbnail, store in cache obviously

		auto absoulutePath = m_Project->GetAssetAbsolutePath(assetPath);
		std::filesystem::file_time_type lastWriteTime = std::filesystem::last_write_time(absoulutePath);
		uint64_t timestamp = std::chrono::duration_cast<std::chrono::seconds>(lastWriteTime.time_since_epoch()).count();

		if (m_CachedImages.find(assetPath) != m_CachedImages.end())
		{
			auto& cachedImage = m_CachedImages.at(assetPath);
			if (cachedImage.Timestamp == timestamp)
				return cachedImage.Image;
		}

		if (assetPath.extension() != ".png")
			return nullptr;

		std::shared_ptr<Texture2D> texture = TextureImporter::LoadTexture2D(absoulutePath);
		if (!texture)
			return nullptr;

		auto& cachedImage = m_CachedImages[assetPath];
		cachedImage.Timestamp = timestamp;
		cachedImage.Image = texture;

		return cachedImage.Image;
	}

}