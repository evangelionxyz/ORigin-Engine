#pragma once
#include "Origin\Project\Project.h"
#include "Origin\Renderer\Texture.h"

namespace origin {

	struct ThumbnailImage
	{
		uint32_t Timestamp;
		std::shared_ptr<Texture2D> Image;
	};

	class ThumbnailCache
	{
	public:
		ThumbnailCache() = default;
		ThumbnailCache(std::shared_ptr<Project> project);

		std::shared_ptr<Texture2D> GetOrCreateThumbnail(const std::filesystem::path path);

	private:
		std::shared_ptr<Project> m_Project;
		std::map<std::filesystem::path, ThumbnailImage> m_CachedImages;
		std::filesystem::path m_ThumbnailCachePath;
	};
}

