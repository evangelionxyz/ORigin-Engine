// Copyright (c) 2023 Evangelion Manuhutu | ORigin Engine

#pragma once

#include "AssetManagerBase.h"

#include "Origin\Project\Project.h"

namespace origin {

	class AssetManager
	{
	public:

		template<typename T>
		static std::shared_ptr<T> GetAsset(AssetHandle handle)
		{
			std::shared_ptr<Asset> asset = Project::GetActive()->GetAssetManager()->GetAsset(handle);
			return std::static_pointer_cast<T>(asset);
		}
	};

}