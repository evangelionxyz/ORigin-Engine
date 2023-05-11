#pragma once

#include "Shader.h"

#include "glm\glm.hpp"
#include <string>

namespace Origin
{
	struct Vertex
	{
		glm::vec3 Position;
		glm::vec3 Normal;
		glm::vec2 TexCoord;
	};

	class Mesh
	{
	public:
		virtual void Draw() = 0;
		static std::shared_ptr<Mesh> Create(const std::vector<Vertex>& vertices, const std::vector<uint32_t>& indices);
	};
}


