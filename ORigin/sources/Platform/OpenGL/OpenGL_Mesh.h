#pragma once

#include "Origin\Renderer\Mesh.h"
#include "Origin\Renderer\VertexArray.h"
#include "Origin\Renderer\Buffer.h"
#include "Origin\Renderer\Shader.h"

#include "glm\glm.hpp"
#include <vector>

namespace Origin
{
	class OpenGLMesh : public Mesh
	{
	public:
		OpenGLMesh() = default;
		OpenGLMesh(const std::vector<Vertex>& vertices, const std::vector<uint32_t>& indices);

		virtual ~OpenGLMesh() override;

		void Draw() override;

		bool IsLoaded() const override;

	private:
		std::shared_ptr<VertexArray> m_VertexArray;
		std::shared_ptr<VertexBuffer> m_VertexBuffer;

		bool m_Loaded = false;
	};
}