// Copyright (c) Evangelion Manuhutu | ORigin Engine
#pragma once
#include "AssetManagerBase.h"

#include "Origin/Project/Project.h"

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

		static bool IsAssetHandleValid(AssetHandle handle)
		{
			return Project::GetActive()->GetAssetManager()->IsAssetHandleValid(handle);
		}

		static bool IsAssetLoaded(AssetHandle handle)
		{
			return Project::GetActive()->GetAssetManager()->IsAssetLoaded(handle);
		}

		static AssetType GetAssetType(AssetHandle handle)
		{
			return Project::GetActive()->GetAssetManager()->GetAssetType(handle);
		}
	};

}