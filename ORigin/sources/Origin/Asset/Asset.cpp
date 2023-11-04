// Copyright (c) 2023 Evangelion Manuhutu | ORigin Engine
#include "pch.h"
#include "Asset.h"

namespace origin {

	std::string_view AssetTypeToString(AssetType type)
	{
		switch (type)
		{
			case origin::AssetType::None: return "AssetType::None";
			case origin::AssetType::Scene: return "AssetType::Scene";
			case origin::AssetType::Texture2D: return "AssetType::Texture2D";
			case origin::AssetType::TextureCube: return "AssetType::TextureCube";
			case origin::AssetType::Audio: return "AssetType::Audio";
			case origin::AssetType::StaticMesh: return "AssetType::StaticMesh";
			case origin::AssetType::Mesh: return "AssetType::Mesh";
			case origin::AssetType::Animation: return "AssetType::Animation";
		}
	}

	AssetType AssetTypeFromtString(std::string_view assetType)
	{
		if (assetType == "AssetType::None") return AssetType::None;
		if (assetType == "AssetType::Scene") return AssetType::Scene;
		if (assetType == "AssetType::Texture2D") return AssetType::Texture2D;
		if (assetType == "AssetType::TextureCube") return AssetType::TextureCube;
		if (assetType == "AssetType::Audio") return AssetType::Audio;
		if (assetType == "AssetType::StaticMesh") return AssetType::StaticMesh;
		if (assetType == "AssetType::Mesh") return AssetType::Mesh;
		if (assetType == "AssetType::Animation") return AssetType::Animation;

		return AssetType::None;
	}

}