#pragma once

#include "Shader.h"
#include "Material.h"

#include "Origin\Asset\Asset.h"
#include "Origin\Scene\EditorCamera.h"
#include "Origin\Scene\SceneCamera.h"

#include <memory>
#include <string>
#include <glm\glm.hpp>

namespace origin
{
	class Model : public Asset
	{
	public:
		virtual ~Model() {}
		virtual void Draw() = 0;
		virtual void Draw(int entityID) = 0;
		virtual void SetMaterial(std::shared_ptr<Material> mat) = 0;
		virtual void RemoveMaterial() = 0;
		virtual void SetTransform(const glm::mat4& transform) = 0;

		virtual const std::filesystem::path& GetFilepath() const = 0;
		virtual std::shared_ptr<Material>& GetMaterial() = 0;

		static std::shared_ptr<Model> Create(const std::filesystem::path &filepath);

		static AssetType GetStaticType() { return AssetType::StaticMesh; }
		AssetType GetType() const override { return GetStaticType(); }
	};
}


