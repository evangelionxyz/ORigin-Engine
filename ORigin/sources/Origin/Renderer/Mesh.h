
#pragma once

#include "Shader.h"
#include "Texture.h"
#include <glm/glm.hpp>
#include <assimp/material.h>

#define MAX_BONE_INFLUENCE 4

namespace origin
{
	struct MeshVertex
	{
		glm::vec3 Position;
		glm::vec3 Normal;
		glm::vec2 TexCoord;
	};

	struct MeshUniformData
	{
		glm::mat4 Transform = glm::mat4(1.0f);
		int EntityId = -1;
	};

	class Mesh
	{
	public:
		virtual ~Mesh() {}
		virtual void Draw(const glm::mat4 &transform, int entityId = -1) = 0;

		static std::shared_ptr<Mesh> Create(const std::vector<MeshVertex>& vertices, const std::vector<uint32_t>& indices);
	};
}


