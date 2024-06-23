#pragma once

#include "Shader.h"
#include "Material.h"
#include "Origin/Asset/Asset.h"
#include "Origin/Scene/EditorCamera.h"
#include "Origin/Scene/SceneCamera.h"
#include "Origin/Scene/Components.h"

#include <memory>
#include <string>
#include <glm\glm.hpp>

namespace origin
{
	class Scene;
	class Model : public Asset
	{
	public:
		virtual ~Model() {}

		virtual void SetTransform(const glm::mat4& transform) = 0;
		virtual void SetMaterial(AssetHandle handle) = 0;
		virtual const std::filesystem::path &GetFilepath() const = 0;
		virtual AssetHandle GetMaterial() = 0;
		virtual std::vector<UUID> GetMeshes() = 0;

		static std::shared_ptr<Model> Create(const std::filesystem::path &filepath, Scene *scene);
		static AssetType GetStaticType() { return AssetType::Model; }
		AssetType GetType() const override { return GetStaticType(); }
	};
}


