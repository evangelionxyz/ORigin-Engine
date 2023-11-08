// Copyright (c) Evangelion Manuhutu | ORigin Engine
#include "pch.h"
#include "SceneImporter.h"

#include "Origin\Project\Project.h"
#include "Origin\Scene\SceneSerializer.h"


namespace origin {

	std::shared_ptr<Scene> SceneImporter::ImportScene(AssetHandle handle, const AssetMetadata& metadata)
	{
		return LoadScene(Project::GetActiveAssetDirectory() / metadata.Filepath);
	}

	std::shared_ptr<Scene> SceneImporter::LoadScene(const std::filesystem::path& path)
	{
		std::shared_ptr<Scene> scene = std::make_shared<Scene>();
		SceneSerializer serializer(scene);
		serializer.Deserialize(path);
		return scene;
	}

	void SceneImporter::SaveScene(std::shared_ptr<Scene> scene, const std::filesystem::path& path)
	{
		SceneSerializer serializer(scene);
		serializer.Serialize(Project::GetActiveAssetDirectory() / path);
	}

}
