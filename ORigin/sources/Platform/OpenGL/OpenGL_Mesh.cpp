// Copyright (c) 2022 Evangelion Manuhutu | ORigin Engine

#include "pch.h"
#include "OpenGL_Mesh.h"
#include "Origin\Renderer\RenderCommand.h"

namespace origin
{
	OpenGLMesh::OpenGLMesh(const std::vector<Vertex>& vertices, const std::vector<uint32_t>& indices)
	{
		OGN_CORE_WARN("MESH INFO:");
		OGN_CORE_WARN("VERTEX");
		OGN_CORE_WARN("size (bytes) : {}", sizeof(Vertex));

		OGN_CORE_WARN("VERTICES");
		OGN_CORE_WARN("count : {}", vertices.size());
		OGN_CORE_WARN("size (bytes) : {}", vertices.size() * sizeof(Vertex));

		OGN_CORE_WARN("INDICES");
		OGN_CORE_WARN("count : {}", indices.size());
		OGN_CORE_WARN("size (bytes) : {}", indices.size() * sizeof(uint32_t));

		m_VertexArray = VertexArray::Create();
		m_VertexBuffer = VertexBuffer::Create(vertices);
		m_VertexBuffer->SetLayout
		({
			{ ShaderDataType::Float3, "aPosition" },
			{ ShaderDataType::Float3, "aNormal"	},
			{ShaderDataType::Float2,	"aTexCoord"	}
		});

		m_VertexArray->AddVertexBuffer(m_VertexBuffer);
		std::shared_ptr<IndexBuffer> indexBuffer = IndexBuffer::Create(indices);
		m_VertexArray->SetIndexBuffer(indexBuffer);

		OGN_CORE_WARN("INDEX COUNT: {}", indexBuffer->GetCount());

		m_Loaded = true;
	}

	OpenGLMesh::~OpenGLMesh()
	{
	}

	void OpenGLMesh::Draw()
	{
		if (!m_VertexArray)
		{
			m_Loaded = false;
			return;
		}

		RenderCommand::DrawIndexed(m_VertexArray);
	}

	bool OpenGLMesh::IsLoaded() const
	{
		return m_Loaded;
	}

}