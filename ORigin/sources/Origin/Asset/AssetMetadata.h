// Copyright (c) Evangelion Manuhutu | ORigin Engine

#pragma once
#include "Asset.h"
#include <filesystem>

namespace origin {

	struct AssetMetadata
	{
		AssetType Type = AssetType::None;
		std::filesystem::path Filepath;

		operator bool() const { return Type != AssetType::None; }
	};

}
