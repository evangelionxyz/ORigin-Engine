// Copyright (c) 2023 Evangelion Manuhutu | ORigin Engine

#pragma once
#include "Origin\Core\UUID.h"

namespace origin {
	
	using AssetHandle = UUID;

	enum class AssetType
	{
		None = -1,
		Scene,
		Texture2D,
		TextureCube,
		Audio,
		StaticMesh,
		Mesh,
		Animation
	};

	class Asset
	{
	public:
		AssetHandle Handle;
		virtual AssetType GetType() const = 0;
	};
}