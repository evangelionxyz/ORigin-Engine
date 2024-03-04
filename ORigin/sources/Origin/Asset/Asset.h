// Copyright (c) Evangelion Manuhutu | ORigin Engine
#pragma once
#include "Origin\Core\UUID.h"

namespace origin {
	
	using AssetHandle = UUID;
	enum class AssetType : uint16_t
	{
		None = 0,
		Scene,
		Texture2D,
		Font,
		TextureCube,
		Audio,
		StaticMesh,
		Mesh,
		Animation,
		SpriteSheet
	};

	const char *AssetTypeToString(AssetType type);
	AssetType AssetTypeFromString(const char *assetType);

	class Asset
	{
	public:
		AssetHandle Handle;
		virtual AssetType GetType() const = 0;
	};
}