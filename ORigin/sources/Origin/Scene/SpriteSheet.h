#pragma once
#include "Origin\Asset\Asset.h"

#include <filesystem>
#include <glm\glm.hpp>

namespace origin
{
	struct SpriteData
	{
		uint32_t Position;
		glm::vec2 Min, Max;
	};

	class SpriteSheet : public Asset
	{
	public:
		SpriteSheet() = default;
		SpriteSheet(const std::filesystem::path &filepath);
		void AddTexture(AssetHandle handle);

		static std::shared_ptr<SpriteSheet> Create();
		static std::shared_ptr<SpriteSheet> Create(const std::filesystem::path &filepath);


		static AssetType GetStaticType() { return AssetType::SpriteSheet; }
		virtual AssetType GetType() const { return GetStaticType(); }

		const AssetHandle GetTextureHandle() const { return m_TextureHandle; }
		std::vector<SpriteData> Sprites;
	private:
		std::filesystem::path m_Filepath;
		AssetHandle m_TextureHandle;
	};
}

