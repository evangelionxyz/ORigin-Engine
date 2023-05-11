#pragma once

#include "Origin\Renderer\Mesh.h"
#include "Origin\Renderer\VertexArray.h"
#include "Origin\Renderer\Buffer.h"

#include "glm\glm.hpp"
#include <vector>

namespace Origin
{
	class OpenGLMesh : public Mesh
	{
	public:
		OpenGLMesh() = default;
		OpenGLMesh(const std::vector<Vertex>& vertices, const std::vector<uint32_t>& indices);
		void Draw() override;

	private:
		std::shared_ptr<VertexArray> m_VertexArray;
		std::shared_ptr<VertexBuffer> m_VertexBuffer;
	};
}