
#pragma once

#include "Shader.h"
#include "Texture.h"

#include <string>
#include <glm\glm.hpp>

#define MAX_BONE_INFLUENCE 4

namespace origin
{
	struct MeshVertex
	{
		glm::vec3 Position;
		glm::vec3 Normal;
		glm::vec2 TexCoord;
	};

	class Mesh
	{
	public:
		virtual ~Mesh() {}
		virtual void Draw(const std::shared_ptr<Shader>& shader) = 0;
		virtual bool IsLoaded() const = 0;

		static std::shared_ptr<Mesh> Create(
			const std::vector<MeshVertex>& vertices,
			const std::vector<uint32_t>& indices,
			const std::vector<std::shared_ptr<Texture2D>>& textures,
			const std::string& modelFilepath = "");
	};
}


