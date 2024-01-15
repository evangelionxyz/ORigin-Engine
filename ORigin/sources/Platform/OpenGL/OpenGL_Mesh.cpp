// Copyright (c) 2022 Evangelion Manuhutu | ORigin Engine

#include "pch.h"
#include "OpenGL_Mesh.h"
#include "Origin\Renderer\Texture.h"
#include "Origin\Renderer\RenderCommand.h"

namespace origin
{
	OpenGLMesh::OpenGLMesh(
		const std::vector<MeshVertex>& vertices, 
		const std::vector<uint32_t>& indices, 
		const std::vector<std::shared_ptr<Texture2D>>& textures,
		const std::string& modelFilepath)
	{
		OGN_CORE_WARN("MESH INFO: \"{}\"", modelFilepath);
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

		m_Textures = textures;
		m_Loaded = true;
	}

	OpenGLMesh::~OpenGLMesh()
	{
	}

	void OpenGLMesh::Draw(const std::shared_ptr<Shader>& shader)
	{
		if (!m_VertexArray)
		{
			m_Loaded = false;
			return;
		}

		for (uint32_t i = 0; i < m_Textures.size(); i++)
		{
			shader->SetInt("u_Texture", i);
			m_Textures[i]->Bind(i);
		}

		RenderCommand::DrawIndexed(m_VertexArray);

		for (auto& tex : m_Textures)
		{
			tex->Unbind();
		}
	}
}