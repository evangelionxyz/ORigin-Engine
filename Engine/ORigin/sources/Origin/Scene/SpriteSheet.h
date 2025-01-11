// Copyright (c) 2022-present Evangelion Manuhutu | ORigin Engine

#ifndef SPRITE_SHEET_H
#define SPRITE_SHEET_H

#include "Origin/Asset/Asset.h"

#include "Origin/Math/Math.h"

#include <filesystem>
#include <glm/glm.hpp>

namespace origin {
struct SpriteSheetData
{
	Rect rect = Rect(0.0f, 0.0f, 1.0f, 1.0f);
	glm::vec2 atlas_size = { 0.0f, 0.0f };
		
	glm::vec2 uv0 = glm::vec2(0.0f);
	glm::vec2 uv1 = glm::vec2(1.0f);
		
	AssetHandle texture_handle = UUID(0);
};

class SpriteSheet : public Asset
{
	public:
		SpriteSheet();

	void SetTexture(AssetHandle handle);

	static Ref<SpriteSheet> Create();

	static AssetType GetStaticType() { return AssetType::SpritesSheet; }
	AssetType GetType() const override { return GetStaticType(); }
	AssetHandle GetTextureHandle() const { return m_TextureHandle; }
	std::vector<SpriteSheetData> Sprites;

	void SetGrid(const glm::vec2 &grid_size) { m_GridSize = grid_size; }

	const glm::vec2 &GetGrid() const { return m_GridSize; }
		
	private:
	glm::vec2 m_GridSize;
	AssetHandle m_TextureHandle;

	friend class SpriteSheetEditorPanel;
};

}

#endif