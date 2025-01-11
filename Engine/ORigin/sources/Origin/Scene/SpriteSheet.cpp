// Copyright (c) 2022-present Evangelion Manuhutu | ORigin Engine

#include "pch.h"
#include "SpriteSheet.h"

namespace origin
{

	SpriteSheet::SpriteSheet()
		: m_GridSize({1.0f, 1.0f})
	{
	}

	void SpriteSheet::SetTexture(AssetHandle handle)
	{
		m_TextureHandle = handle;
	}

	std::shared_ptr<SpriteSheet> SpriteSheet::Create()
	{
		return std::make_shared<SpriteSheet>();
	}

}
