// Copyright (c) Evangelion Manuhutu | ORigin Engine
#pragma once
#include "Asset.h"
#include "AssetMetadata.h"

#include "Origin\Scene\Scene.h"

namespace origin {

	class SceneImporter
	{
	public:
		static std::shared_ptr<Scene> ImportScene(AssetHandle handle, const AssetMetadata& metadata);
		static std::shared_ptr<Scene> LoadScene(const std::filesystem::path& path);
		static void SaveScene(std::shared_ptr<Scene> scene, const std::filesystem::path& path);
	};
}

