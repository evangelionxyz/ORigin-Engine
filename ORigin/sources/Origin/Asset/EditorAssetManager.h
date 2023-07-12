// Copyright (c) 2023 Evangelion Manuhutu | ORigin Engine

#pragma once
#include "AssetManagerBase.h"

#include <map>

namespace origin {

	using AssetMap = std::map < AssetHandle, std::shared_ptr<Asset>>;

	class EditorAssetManager : public AssetManagerBase
	{
	public:
		std::shared_ptr<Asset> GetAsset(AssetHandle handle) const override;

	private:
		AssetMap m_LoadedAssets;
	};
}