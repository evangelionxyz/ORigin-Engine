// Copyright (c) Evangelion Manuhutu | ORigin Engine
#pragma once
#include "Origin\Core\UUID.h"
#include <string>

namespace origin {
	
	using AssetHandle = UUID;
	enum class AssetType : uint16_t
	{
		None = 0,
		Scene,
		Texture,
		Font,
		Audio,
		StaticMesh,
		Material,
		MeshSource,
		Animation,
		SpritesSheet
	};

	const char *AssetTypeToString(AssetType type);
	AssetType AssetTypeFromString(const std::string_view &assetType);

	class Asset
	{
	public:
		AssetHandle Handle;
		virtual AssetType GetType() const = 0;
	};
}