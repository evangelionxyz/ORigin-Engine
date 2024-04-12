// Copyright (c) Evangelion Manuhutu | ORigin Engine

#include "pch.h"
#include "OpenGL_Mesh.h"
#include "Origin\Renderer\Renderer.h"
#include "Origin\Renderer\Texture.h"
#include "Origin\Renderer\RenderCommand.h"

#pragma warning(disable : OGN_DISABLED_WARNINGS)

namespace origin
{
	OpenGLMesh::OpenGLMesh(const std::vector<MeshVertex>& vertices, const std::vector<uint32_t>& indices)
	{
		PROFILER_RENDERING();

		OGN_CORE_TRACE("VERTEX");
		OGN_CORE_TRACE("	Size : {} bytes", sizeof(MeshVertex));

		OGN_CORE_TRACE("VERTICES");
		OGN_CORE_TRACE("	Count: {}", vertices.size());
		OGN_CORE_TRACE("	Size : {} bytes", vertices.size() * sizeof(MeshVertex));

		OGN_CORE_TRACE("INDICES");
		OGN_CORE_TRACE("	Count: {}", indices.size());
		OGN_CORE_TRACE("	Size : {} bytes", indices.size() * sizeof(uint32_t));

		m_VertexArray = VertexArray::Create();
		m_VertexBuffer = VertexBuffer::Create((void*)vertices.data(), vertices.size() * sizeof(MeshVertex));

		m_VertexBuffer->SetLayout
		({
			{ ShaderDataType::Float3, "aPosition"		},
			{ ShaderDataType::Float3, "aNormal"			},
			{	ShaderDataType::Float2,	"aTexCoord"		}
		});

		m_VertexArray->AddVertexBuffer(m_VertexBuffer);

		std::shared_ptr<IndexBuffer> indexBuffer = IndexBuffer::Create(indices);
		m_VertexArray->SetIndexBuffer(indexBuffer);

		OGN_CORE_WARN("INDEX COUNT: {}", indexBuffer->GetCount());
	}

	OpenGLMesh::~OpenGLMesh()
	{
	}

	void OpenGLMesh::Draw()
	{
		PROFILER_RENDERING();

		RenderCommand::DrawIndexed(m_VertexArray);
	}

}