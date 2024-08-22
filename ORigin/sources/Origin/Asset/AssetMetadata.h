// Copyright (c) 2022-present Evangelion Manuhutu | ORigin Engine

#ifndef ASSET_METADATA_H
#define ASSET_METADATA_H

#include "Asset.h"
#include <filesystem>

namespace origin {

    struct AssetMetadata
    {
        AssetType             Type = AssetType::None;
        std::filesystem::path Filepath;
        operator bool() const { return Type != AssetType::None; }
   };
}

#endif