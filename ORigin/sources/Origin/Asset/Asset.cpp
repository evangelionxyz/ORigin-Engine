// Copyright (c) Evangelion Manuhutu | ORigin Engine
#include "pch.h"

#include "Asset.h"
#include "Origin/Core/Assert.h"

namespace origin {

	const char *AssetTypeToString(AssetType type)
	{
		switch (type)
		{
			case AssetType::None: return "AssetType::None";
			case AssetType::Scene: return "AssetType::Scene";
			case AssetType::Texture: return "AssetType::Texture";
			case AssetType::Font: return "AssetType::Font";
			case AssetType::Audio: return "AssetType::Audio";
			case AssetType::Model: return "AssetType::Model";
			case AssetType::Material: return "AssetType::Material";
			case AssetType::MeshSource: return "AssetType::MeshSource";
			case AssetType::Animation: return "AssetType::Animation";
			case AssetType::SpritesSheet: return "AssetType::SpritesSheet";
		}
		OGN_CORE_ASSERT(false, "AssetType: Unkown Asset Type");
		return "AssetType::Invalid";
	}

	AssetType AssetTypeFromString(const std::string_view &assetType)
	{
		if (assetType == "AssetType::None") return AssetType::None;
		if (assetType == "AssetType::Scene") return AssetType::Scene;
		if (assetType == "AssetType::Texture") return AssetType::Texture;
		if (assetType == "AssetType::Font") return AssetType::Font;
		if (assetType == "AssetType::Audio") return AssetType::Audio;
		if (assetType == "AssetType::Model") return AssetType::Model;
		if (assetType == "AssetType::Material") return AssetType::Material;
		if (assetType == "AssetType::MeshSource") return AssetType::MeshSource;
		if (assetType == "AssetType::Animation") return AssetType::Animation;
		if (assetType == "AssetType::SpritesSheet") return AssetType::SpritesSheet;
		return AssetType::None;
	}
}