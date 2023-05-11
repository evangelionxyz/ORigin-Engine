#pragma once

#include "Shader.h"

#include <memory>
#include <string>
#include <glm\glm.hpp>

namespace Origin
{
	class Model
	{
	public:
		virtual void Draw(const glm::mat4& viewProjection) = 0;
		virtual std::shared_ptr<Shader>& GetShader() = 0;
		static std::shared_ptr<Model> Create(const std::string& filepath, std::shared_ptr<Shader>& shader);
		static std::shared_ptr<Model> Create(std::shared_ptr<Shader>& shader);
	};
}


