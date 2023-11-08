// Copyright (c) Evangelion Manuhutu | ORigin Engine
#pragma once
#include "AssetMetadata.h"

namespace origin {

	class AssetImporter
	{
	public:
		static std::shared_ptr<Asset> ImportAsset(AssetHandle handle, const AssetMetadata& metadata);
	};

}
