// Copyright (c) 2022-present Evangelion Manuhutu | ORigin Engine

#ifndef SPRITE_SHEET_H
#define SPRITE_SHEET_H

#include "Origin/Asset/Asset.h"

#include "Origin/Math/Math.h"

#include <filesystem>
#include <glm/glm.hpp>

namespace origin
{
	struct SpriteSheetData
	{
		Rect rect = Rect(0.0f, 0.0f, 1.0f, 1.0f);
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
		AssetType GetType() const override { return GetStaticType(); }
		AssetHandle GetTextureHandle() const { return m_TextureHandle; }
		std::vector<SpriteSheetData> Sprites;
	private:
		std::filesystem::path m_Filepath;
		AssetHandle m_TextureHandle;
	};
}

#endif