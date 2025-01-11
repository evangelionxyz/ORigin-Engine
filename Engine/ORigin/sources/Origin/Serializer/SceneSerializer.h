// Copyright (c) 2022-present Evangelion Manuhutu | ORigin Engine

#ifndef SCENE_SERIALIZER_H
#define SCENE_SERIALIZER_H

#include "Origin/Scene/Scene.h"
#include <filesystem>

#include "yaml-cpp/emitter.h"

namespace origin {

class SceneSerializer
{
public:
	SceneSerializer(const std::shared_ptr<Scene>& scene);
	void Serialize(const std::filesystem::path& filepath);
	void SerializeRuntime(const std::filesystem::path& filepath);

	bool Deserialize(const std::filesystem::path& filepath);
	bool DeserializeRuntime(const std::filesystem::path& filepath);

	void SerializeDeletedEntity(Entity entity, const std::filesystem::path &path);
	void DeserializeDeletedEntity(const std::filesystem::path &path);

private:
	std::shared_ptr<Scene> m_Scene;
};
}

#endif
