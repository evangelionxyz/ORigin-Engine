// Copyright (c) Evangelion Manuhutu | ORigin Engine
#include "pch.h"
#include "AssetImporter.h"

#include "AudioImporter.h"
#include "SceneImporter.h"
#include "TextureImporter.h"

#include "Asset.h"

namespace origin {

	using AssetImportFunction = std::function<std::shared_ptr<Asset>(AssetHandle, const AssetMetadata&)>;

	// Functions to Importing and Loading assets
	static std::map<AssetType, AssetImportFunction> s_AssetImportFunctions = {
		{	AssetType::Audio, AudioImporter::ImportAudio },
		{	AssetType::Texture2D, TextureImporter::ImportTexture2D },
		{	AssetType::Scene, SceneImporter::ImportScene }
	};
	
	// It is automatically match Functions by checking the metadata
	// AssetHandle handle = desiredAssetHandle
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
