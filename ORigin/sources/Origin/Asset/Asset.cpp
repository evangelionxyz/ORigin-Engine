// Copyright (c) Evangelion Manuhutu | ORigin Engine

#include "pch.h"
#include "Asset.h"

namespace origin {

	const char *AssetTypeToString(AssetType type)
	{
		switch (type)
		{
			case AssetType::None: return "AssetType::None";
			case AssetType::Scene: return "AssetType::Scene";
			case AssetType::Texture2D: return "AssetType::Texture2D";
			case AssetType::Font: return "AssetType::Font";
			case AssetType::TextureCube: return "AssetType::TextureCube";
			case AssetType::Audio: return "AssetType::Audio";
			case AssetType::StaticMesh: return "AssetType::StaticMesh";
			case AssetType::Mesh: return "AssetType::Mesh";
			case AssetType::Animation: return "AssetType::Animation";
			case AssetType::SpriteSheet: return "AssetType::SpriteSheet";
		}
		OGN_CORE_ASSERT(false, "AssetType: Unkown Asset Type");
		return "AssetType::Invalid";
	}

	AssetType AssetTypeFromString(const char *assetType)
	{
		if (assetType == "AssetType::None") return AssetType::None;
		if (assetType == "AssetType::Scene") return AssetType::Scene;
		if (assetType == "AssetType::Texture2D") return AssetType::Texture2D;
		if (assetType == "AssetType::Font") return AssetType::Font;
		if (assetType == "AssetType::TextureCube") return AssetType::TextureCube;
		if (assetType == "AssetType::Audio") return AssetType::Audio;
		if (assetType == "AssetType::StaticMesh") return AssetType::StaticMesh;
		if (assetType == "AssetType::Mesh") return AssetType::Mesh;
		if (assetType == "AssetType::Animation") return AssetType::Animation;
		if (assetType == "AssetType::SpriteSheet") return AssetType::SpriteSheet;
		return AssetType::None;
	}
}