#pragma once
#include "Origin/Animation/ModelAnimation.h"
#include "MeshVertexData.h"

#include <filesystem>

namespace origin
{
	class StaticModel
	{
	public:
		StaticModel() = default;
		StaticModel(const std::filesystem::path& filepath);

		const std::vector<Ref<StaticMeshData>> &GetMeshes() { return m_Meshes;  }

	private:
		std::vector<Ref<StaticMeshData>> m_Meshes;
	};

	class SkeletalModel
	{
	public:
		SkeletalModel() = default;
		SkeletalModel(const std::filesystem::path &filepath);

		const std::vector<ModelAnimation>& GetAnimations() { return m_Animations; }
		const std::vector<Ref<MeshData>>& GetMeshes() { return m_Meshes; }

	private:
		std::vector<ModelAnimation> m_Animations;
		std::vector<Ref<MeshData>> m_Meshes;
	};
}
