// Copyright (c) Evangelion Manuhutu | ORigin Engine
#pragma once
#include "AssetManagerBase.h"
#include "AssetMetadata.h"
#include <map>

namespace origin {

	using AssetRegistry = std::map<AssetHandle, AssetMetadata>;

	class EditorAssetManager : public AssetManagerBase
	{
	public:
		std::shared_ptr<Asset> GetAsset(AssetHandle handle) override;

		virtual bool IsAssetHandleValid(AssetHandle handle) const override;
		virtual bool IsAssetLoaded(AssetHandle handle) const override;
		virtual AssetType GetAssetType(AssetHandle handle) const override;

		AssetHandle ImportAsset(const std::filesystem::path& filepath);
		void InsertAsset(AssetHandle handle, AssetMetadata metadata, std::function<std::shared_ptr<Asset>()> loader);

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