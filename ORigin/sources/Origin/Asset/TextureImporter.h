// Copyright (c) Evangelion Manuhutu | ORigin Engine
#pragma once
#include "Asset.h"
#include "AssetMetadata.h"

#include "Origin\Renderer\Texture.h"

namespace origin {

	class TextureImporter
	{
	public:
		static std::shared_ptr<Texture2D> ImportTexture2D(AssetHandle handle, const AssetMetadata& metadata);
		static std::shared_ptr<Texture2D> LoadTexture2D(const std::filesystem::path& path);
	};
}

