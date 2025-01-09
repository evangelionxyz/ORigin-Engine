// Copyright (c) 2022-present Evangelion Manuhutu | ORigin Engine

#include "pch.h"
#include "SpriteSheet.h"

namespace origin
{

	SpriteSheet::SpriteSheet(const std::filesystem::path &filepath)
		: m_Filepath(filepath), m_GridSize({1.0f, 1.0f})
	{
	}

	void SpriteSheet::SetMainTexture(AssetHandle handle)
	{
		m_TextureHandle = handle;
	}

	std::shared_ptr<SpriteSheet> SpriteSheet::Create(const std::filesystem::path &filepath)
	{
		return std::make_shared<SpriteSheet>(filepath);
	}

	std::shared_ptr<SpriteSheet> SpriteSheet::Create()
	{
		return std::make_shared<SpriteSheet>();
	}

}
