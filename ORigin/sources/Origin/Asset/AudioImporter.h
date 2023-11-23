// Copyright (c) Evangelion Manuhutu | ORigin Engine
#pragma once
#include "Asset.h"
#include "AssetMetadata.h"

#include "Origin\Audio\Audio.h"

namespace origin {
	class AudioImporter
	{
	public:
		static std::shared_ptr<Audio> ImportAudio(AssetHandle handle, AssetMetadata metadata);
		static std::shared_ptr<Audio> LoadAudio(const std::filesystem::path filepath, AudioConfig config);
	};
}


