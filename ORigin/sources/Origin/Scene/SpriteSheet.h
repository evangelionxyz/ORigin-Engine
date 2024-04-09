#pragma once
#include "Origin\Asset\Asset.h"

#include <filesystem>
#include <glm\glm.hpp>

namespace origin
{
	struct SpriteSheetData
	{
		glm::vec2 Min = glm::vec2(0.0f);
		glm::vec2 Max = glm::vec2(0.0f);
		AssetHandle TextureHandle = 0;
	};

	class SpriteSheet : public Asset
	{
	public:
		SpriteSheet() = default;
		SpriteSheet(const std::filesystem::path &filepath);

		void SetMainTexture(AssetHandle handle);

		static std::shared_ptr<SpriteSheet> Create();
		static std::shared_ptr<SpriteSheet> Create(const std::filesystem::path &filepath);

		static AssetType GetStaticType() { return AssetType::SpriteSheet; }
		virtual AssetType GetType() const { return GetStaticType(); }

		const AssetHandle GetTextureHandle() const { return m_TextureHandle; }
		std::vector<SpriteSheetData> Sprites;
	private:
		std::filesystem::path m_Filepath;
		AssetHandle m_TextureHandle;
	};
}

