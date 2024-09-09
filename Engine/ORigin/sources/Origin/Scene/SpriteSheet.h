// Copyright (c) 2022-present Evangelion Manuhutu | ORigin Engine

#ifndef SPRITE_SHEET_H
#define SPRITE_SHEET_H

#include "Origin/Asset/Asset.h"

#include <filesystem>
#include <glm/glm.hpp>

namespace origin
{
	struct SpriteSheetData
	{
		glm::vec2 Min = glm::vec2(0.0f);
		glm::vec2 Max = glm::vec2(1.0f);
		AssetHandle TextureHandle = UUID(0);
	};

	class SpriteSheet : public Asset
	{
	public:
		SpriteSheet() = default;
		explicit SpriteSheet(const std::filesystem::path &filepath);

		void SetMainTexture(AssetHandle handle);

		static std::shared_ptr<SpriteSheet> Create();
		static std::shared_ptr<SpriteSheet> Create(const std::filesystem::path &filepath);

		static AssetType GetStaticType() { return AssetType::SpritesSheet; }
		[[nodiscard]] AssetType GetType() const override { return GetStaticType(); }

		[[nodiscard]] AssetHandle GetTextureHandle() const { return m_TextureHandle; }
		std::vector<SpriteSheetData> Sprites;
	private:
		std::filesystem::path m_Filepath;
		AssetHandle m_TextureHandle;
	};
}

#endif