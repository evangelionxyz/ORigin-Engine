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

		VertexArray = VertexArray::Create();
		VertexBuffer = VertexBuffer::Create((void*)vertices.data(), vertices.size() * sizeof(MeshVertex));

		VertexBuffer->SetLayout
		({
			{ ShaderDataType::Float3, "aPosition"		},
			{ ShaderDataType::Float3, "aNormal"			},
			{	ShaderDataType::Float2,	"aTexCoord"		}
		});

		VertexArray->AddVertexBuffer(VertexBuffer);

		std::shared_ptr<IndexBuffer> indexBuffer = IndexBuffer::Create(indices);
		VertexArray->SetIndexBuffer(indexBuffer);

		OGN_CORE_WARN("INDEX COUNT: {}", indexBuffer->GetCount());

		// Create uniform buffer
		int uniformBinding = 0;
		UniformBuffer = UniformBuffer::Create(sizeof(UniformData), uniformBinding);
	}

	OpenGLMesh::~OpenGLMesh()
	{
	}

	void OpenGLMesh::Draw(const glm::mat4 &transform, int entityId)
	{
		UniformData.Transform = transform;
		UniformData.EntityId = entityId;

		VertexBuffer->Bind();
		UniformBuffer->Bind();
		RenderCommand::DrawIndexed(VertexArray);
		UniformBuffer->Unbind();
	}
}