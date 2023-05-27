#pragma once

#include "Shader.h"
#include "Origin\Scene\EditorCamera.h"
#include "Origin\Scene\Camera.h"

#include <memory>
#include <string>
#include "glm\glm.hpp"

namespace Origin
{
	class Model
	{
	public:
		virtual ~Model() {}
		virtual void Draw() = 0;
		virtual void Draw(const EditorCamera& camera) = 0;
		virtual void Draw(const glm::mat4& transform, const EditorCamera& camera, int entityID = -1) = 0;
		virtual void Draw(const glm::mat4& transform, const Camera* camera, int entityID = -1) = 0;
		virtual void LoadLighting(const glm::vec3& position, const glm::vec4& color, float ambient) = 0;

		virtual std::shared_ptr<Shader>& GetShader() = 0;
		static std::shared_ptr<Model> Create(const std::string& filepath, std::shared_ptr<Shader>& shader);
		static std::shared_ptr<Model> Create(std::shared_ptr<Shader>& shader);
	};
}


