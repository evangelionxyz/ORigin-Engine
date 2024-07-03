// Copyright 2022 Evangelion Manuhutu | ORigin Engine

#pragma once
#include "Origin/Scene/Scene.h"
#include <filesystem>

namespace origin
{
	class SceneSerializer
	{
	public:
		SceneSerializer(const std::shared_ptr<Scene>& scene);
		void Serialize(const std::filesystem::path& filepath);
		void SerializeRuntime(const std::filesystem::path& filepath);

		bool Deserialize(const std::filesystem::path& filepath);
		bool DeserializeRuntime(const std::filesystem::path& filepath);

	private:
		std::shared_ptr<Scene> m_Scene;
	};
}


