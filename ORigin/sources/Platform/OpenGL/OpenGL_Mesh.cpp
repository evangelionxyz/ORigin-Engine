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
		//m_VertexBuffer = VertexBuffer::Create(vertices.size() * sizeof(MeshVertex));
		m_VertexBuffer = VertexBuffer::Create(vertices);

		m_VertexBuffer->SetLayout
		({
			{ ShaderDataType::Float3, "aPosition"		},
			{ ShaderDataType::Float3, "aNormal"			},
			{	ShaderDataType::Float2,	"aTexCoord"		},
			{	ShaderDataType::Float3,	"aTangents"		},
			{	ShaderDataType::Float3,	"aBitangents"	},
			{	ShaderDataType::Int4,		"aBones"			},
			{	ShaderDataType::Float4,	"aWeights"		}
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

		uint32_t diffuseNumber = 1;
		uint32_t specularNumber = 1;
		uint32_t normalNumber = 1;
		uint32_t heightNumber = 1;

		for (uint32_t i = 0; i < m_Textures.size(); i++)
		{
			std::string number;
			std::string name = m_Textures[i]->GetMaterialTypeName();

			if (name == "texture_diffuse")
				number = std::to_string(diffuseNumber++);
			else if (name == "texture_specular")
				number = std::to_string(specularNumber++);
			else if (name == "texture_normal")
				number = std::to_string(normalNumber++);
			else if (name == "texture_height")
				number = std::to_string(heightNumber++);

			shader->SetInt("material."+name+number, i);
			m_Textures[i]->Bind(i);
		}

		// Draw
		RenderCommand::DrawIndexed(m_VertexArray);

		for (auto& tex : m_Textures)
			tex->Unbind();
	}
}