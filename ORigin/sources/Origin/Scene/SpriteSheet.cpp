#include "pch.h"
#include "SpriteSheet.h"

namespace origin
{

	SpriteSheet::SpriteSheet(const std::filesystem::path &filepath)
		: m_Filepath(filepath)
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
