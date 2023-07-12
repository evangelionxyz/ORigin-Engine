// Copyright (c) 2023 Evangelion Manuhutu | ORigin Engine

#pragma once

#include "Asset.h"

namespace origin {

	class AssetManagerBase
	{
	public:
		virtual std::shared_ptr<Asset> GetAsset(AssetHandle handle) const = 0;
	};

}