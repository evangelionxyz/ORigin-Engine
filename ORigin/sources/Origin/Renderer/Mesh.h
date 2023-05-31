#pragma once

#include "Shader.h"

#include "glm\glm.hpp"
#include <string>

namespace origin
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
		virtual ~Mesh() {}
		virtual void Draw() = 0;
		virtual bool IsLoaded() const = 0;

		static std::shared_ptr<Mesh> Create(const std::vector<Vertex>& vertices, const std::vector<uint32_t>& indices);

	};
}


