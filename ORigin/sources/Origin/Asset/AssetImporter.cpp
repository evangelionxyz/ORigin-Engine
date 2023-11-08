// Copyright (c) Evangelion Manuhutu | ORigin Engine
#include "pch.h"
#include "AssetImporter.h"

#include "SceneImporter.h"
#include "TextureImporter.h"

#include "Asset.h"

namespace origin {

	using AssetImportFunction = std::function<std::shared_ptr<Asset>(AssetHandle, const AssetMetadata&)>;
	static std::map<AssetType, AssetImportFunction> s_AssetImportFunctions = {
		{	AssetType::Texture2D, TextureImporter::ImportTexture2D },
		{	AssetType::Scene, SceneImporter::ImportScene }
	};

	std::shared_ptr<Asset> AssetImporter::ImportAsset(AssetHandle handle, const AssetMetadata& metadata)
	{
		if (s_AssetImportFunctions.find(metadata.Type) == s_AssetImportFunctions.end())
		{
			OGN_CORE_ERROR("No Importer available for asset type: {} ", AssetTypeToString(metadata.Type));
			return nullptr;
		}

		return s_AssetImportFunctions.at(metadata.Type)(handle, metadata);
	}

}
