// Copyright (c) 2022 Evangelion Manuhutu | ORigin Engine
#pragma once
#include "Origin\Renderer\Mesh.h"
#include "Origin\Renderer\VertexArray.h"
#include "Origin\Renderer\Buffer.h"

#include <vector>
#include <map>

namespace origin
{
	class OpenGLMesh : public Mesh
	{
	public:
		OpenGLMesh() = default;

		OpenGLMesh(const std::vector<MeshVertex>& vertices, const std::vector<uint32_t>& indices);
		~OpenGLMesh() override;
		void Draw() override;

	private:
		std::shared_ptr<VertexArray> m_VertexArray;
		std::shared_ptr<VertexBuffer> m_VertexBuffer;
	};
}