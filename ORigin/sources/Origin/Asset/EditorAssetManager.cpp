// Copyright (c) 2023 Evangelion Manuhutu | ORigin Engine

#include "pch.h"
#include "EditorAssetManager.h"

namespace origin {

	std::shared_ptr<Asset> EditorAssetManager::GetAsset(AssetHandle handle) const
	{
		if (m_LoadedAssets.find(handle) == m_LoadedAssets.end())
		{
			OGN_CORE_ERROR("EditoAssetManager: No Asset Found");
			return nullptr;
		}

		return m_LoadedAssets.at(handle);

	}
}