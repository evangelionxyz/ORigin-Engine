// Copyright (c) 2022-present Evangelion Manuhutu | ORigin Engine

#ifndef ASSET_MANAGER_H
#define ASSET_MANAGER_H

#include "AssetManagerBase.h"
#include "Origin/Project/Project.h"

namespace origin {

class AssetManager
{
public:
    template<typename T>
    static Ref<T> GetAsset(AssetHandle handle)
    {
        Ref<Asset> asset = Project::GetActive()->GetAssetManager()->GetAsset(handle);
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

#endif