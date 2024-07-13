
#pragma once

#include "Shader.h"
#include "Texture.h"
#include <glm/glm.hpp>
#include <assimp/material.h>

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

	class VertexArray;
	class VertexBuffer;
	class UniformBuffer;
	
	class Mesh
	{
	public:
		virtual ~Mesh() {}
		virtual void Draw(const glm::mat4 &transform, int entityId = -1) = 0;

		std::shared_ptr<VertexArray> VertexArray;
		std::shared_ptr<VertexBuffer> VertexBuffer;
		std::shared_ptr<UniformBuffer> UniformBuffer;
		MeshUniformData UniformData;
		
		static std::shared_ptr<Mesh> Create(const std::vector<MeshVertex>& vertices, const std::vector<uint32_t>& indices);
	};
}


