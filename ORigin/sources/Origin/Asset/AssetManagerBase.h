// Copyright (c) Evangelion Manuhutu | ORigin Engine

#pragma once
#include "Asset.h"

#include <memory>
#include <map>

namespace origin {

	using AssetMap = std::map<AssetHandle, std::shared_ptr<Asset>>;

	class AssetManagerBase
	{
	public:
		virtual std::shared_ptr<Asset> GetAsset(AssetHandle handle) = 0;

		virtual bool IsAssetHandleValid(AssetHandle handle) const = 0;
		virtual bool IsAssetLoaded(AssetHandle handle) const = 0;
		virtual AssetType GetAssetType(AssetHandle handle) const = 0;
	};

}