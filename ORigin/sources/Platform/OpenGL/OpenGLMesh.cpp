// Copyright (c) Evangelion Manuhutu | ORigin Engine

#include "pch.h"
#include "OpenGLMesh.h"
#include "Origin/Renderer/Texture.h"
#include "Origin/Renderer/Renderer.h"
#include "Origin/Renderer/UniformBuffer.h"
#include "Origin/Renderer/RenderCommand.h"

#include "Origin/Profiler/Profiler.h"

namespace origin
{
	OpenGLMesh::OpenGLMesh(const std::vector<MeshVertex>& vertices, const std::vector<uint32_t>& indices)
	{
		OGN_PROFILER_RENDERING();

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

		// Create uniform buffer
		int uniformBinding = 0;
		m_UniformBuffer = UniformBuffer::Create(sizeof(m_UniformData), uniformBinding);
	}

	OpenGLMesh::~OpenGLMesh()
	{
	}

	void OpenGLMesh::Draw(const glm::mat4 &transform, int entityId)
	{
		m_UniformData.Transform = transform;
		m_UniformData.EntityId = entityId;

		m_UniformBuffer->Bind();
		RenderCommand::DrawIndexed(m_VertexArray);
		m_UniformBuffer->Unbind();
	}
	
}