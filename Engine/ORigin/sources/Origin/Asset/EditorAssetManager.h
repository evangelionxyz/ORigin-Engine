// Copyright (c) 2022-present Evangelion Manuhutu | ORigin Engine

#ifndef EDITOR_ASSET_MANAGER_H
#define EDITOR_ASSET_MANAGER_H

#include "AssetManagerBase.h"
#include "AssetMetadata.h"
#include <map>

namespace origin {

    using AssetRegistry = std::map<AssetHandle, AssetMetadata>;

    class EditorAssetManager : public AssetManagerBase
    {
    public:
        Ref<Asset> GetAsset(AssetHandle handle) override;

        bool IsAssetHandleValid(AssetHandle handle) const override;
        bool IsAssetLoaded(AssetHandle handle) const override;
        bool IsAssetLoaded(const std::string &filepath) const override;
        AssetType GetAssetType(AssetHandle handle) const override;

        AssetHandle ImportAsset(const std::filesystem::path& filepath);
        void InsertAsset(AssetHandle handle, AssetMetadata metadata, std::function<Ref<Asset>()> loader);

        void RemoveAsset(AssetHandle handle);
        void RemoveLoadedAsset(AssetHandle handle);

        const AssetMetadata& GetMetadata(AssetHandle handle) const;
        const std::filesystem::path& GetFilepath(AssetHandle handle);

        AssetRegistry &GetAssetRegistry() { return m_AssetRegistry; }

        void SerializeAssetRegistry();
        bool DeserializeAssetRegistry();

    private:
        AssetRegistry m_AssetRegistry;
        AssetMap m_LoadedAssets;
    };
}

#endif