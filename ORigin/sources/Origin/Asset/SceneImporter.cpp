// Copyright (c) Evangelion Manuhutu | ORigin Engine
#include "pch.h"
#include "SceneImporter.h"

#include "AssetManager.h"

#include "Origin\Project\Project.h"
#include "Origin\Scene\SceneSerializer.h"

#include "Origin\Utils\PlatformUtils.h"

namespace origin {

	std::shared_ptr<Scene> SceneImporter::ImportScene(AssetHandle handle, const AssetMetadata& metadata)
	{
		return LoadScene(Project::GetActiveAssetDirectory() / metadata.Filepath);
	}

	std::shared_ptr<Scene> SceneImporter::LoadScene(const std::filesystem::path& filepath)
	{
		std::shared_ptr<Scene> scene = std::make_shared<Scene>();
		SceneSerializer serializer(scene);
		serializer.Deserialize(filepath);
		return scene;
	}

	AssetHandle SceneImporter::OpenScene(const std::filesystem::path& filepath)
	{
		auto relativePath = std::filesystem::relative(filepath, Project::GetActiveAssetDirectory());

		if (!relativePath.empty())
		{
			auto& assetRegistry = Project::GetActive()->GetEditorAssetManager()->GetAssetRegistry();
			for (auto a : assetRegistry)
			{
				if (relativePath.generic_string() == a.second.Filepath)
					return a.first;
			}
		}

		OGN_CORE_ASSERT(false, "SceneImporter: Scene shoule be imported");
		return 0;
	}

	void SceneImporter::SaveScene(std::shared_ptr<Scene> scene, const std::filesystem::path& path)
	{
		SceneSerializer serializer(scene);
		serializer.Serialize(Project::GetActiveAssetDirectory() / path);
	}

}
