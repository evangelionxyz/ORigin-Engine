#pragma once

#include "Shader.h"
#include "Material.h"

#include "Origin\Scene\EditorCamera.h"
#include "Origin\Scene\SceneCamera.h"

#include <memory>
#include <string>
#include <glm\glm.hpp>

namespace origin
{
	class Model
	{
	public:
		virtual ~Model() {}
		virtual void Draw() = 0;
		virtual void Draw(const glm::mat4& modelTransform, const EditorCamera& camera, int entityID = -1) = 0;
		virtual void Draw(const glm::mat4& modelTransform, const SceneCamera& camera, const glm::mat4& cameraTransform, int entityID = -1) = 0;
		virtual const std::string& GetFilepath() const = 0;

		static std::shared_ptr<Model> Create(const std::string& filepath, std::shared_ptr<Material> material);
	};
}


