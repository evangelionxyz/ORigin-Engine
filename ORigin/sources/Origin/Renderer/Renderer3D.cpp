// Copyright (c) 2022 Evangelion Manuhutu | ORigin Engine

#include "pch.h"
#include "RenderCommand.h"
#include "Renderer.h"
#include "Renderer3D.h"

#include <glm\gtc\matrix_transform.hpp>


namespace Origin
{
	struct Render3DData
	{
		const uint32_t MaxTriangles = 1000;
		uint32_t MaxVertices = 4 * MaxTriangles;
		uint32_t MaxIndices = 6 * MaxTriangles;
		static const uint32_t MaxTextureSlots = 32;
	};

	static Render3DData s_RenderData;

	struct CubeVertex
	{
		glm::vec3 Position;
		glm::vec4 Color;
	};

	struct Renderer3D_Data
	{
		// Cube Data
		std::shared_ptr<Shader> cubeShader;
		std::shared_ptr<VertexArray> cubeVertexArray;
		std::shared_ptr<VertexBuffer> cubeVertexBuffer;

		std::shared_ptr<Texture3D> WhiteTexture;
		std::array<std::shared_ptr<Texture3D>, s_RenderData.MaxTextureSlots> TextureSlots;
		uint32_t TextureSlotIndex = 1;

		uint32_t cubeIndexCount = 0;
		CubeVertex* cubeVertexBufferBase = nullptr;
		CubeVertex* cubeVertexBufferPtr = nullptr;
		glm::vec4 cubeVertexPosition[24];
	};

	static Renderer3D_Data s_3Ddata;

	void Renderer3D::Init()
	{
		s_3Ddata.cubeVertexArray = VertexArray::Create();
		s_3Ddata.cubeVertexBuffer = VertexBuffer::Create(s_RenderData.MaxVertices * sizeof(CubeVertex));

		BufferLayout layout = {
			{ShaderDataType::Float3, "aPos"},
			{ShaderDataType::Float4, "aColor"},
		}; s_3Ddata.cubeVertexBuffer->SetLayout(layout);

		s_3Ddata.cubeVertexArray->AddVertexBuffer(s_3Ddata.cubeVertexBuffer);

		s_3Ddata.cubeVertexBufferBase = new CubeVertex[s_RenderData.MaxVertices];

		uint32_t* CubeIndices = new uint32_t[s_RenderData.MaxIndices];
		uint32_t Offset = 0;
		for (uint32_t i = 0; i < s_RenderData.MaxIndices; i += 6)
		{
			CubeIndices[i + 0] = Offset + 0;
			CubeIndices[i + 1] = Offset + 1;
			CubeIndices[i + 2] = Offset + 2;

			CubeIndices[i + 3] = Offset + 2;
			CubeIndices[i + 4] = Offset + 3;
			CubeIndices[i + 5] = Offset + 0;

			Offset += 4;
		}

		std::shared_ptr<IndexBuffer> indexBuffer = IndexBuffer::Create(CubeIndices, s_RenderData.MaxIndices);
		s_3Ddata.cubeVertexArray->SetIndexBuffer(indexBuffer);
		delete[] CubeIndices;

		s_3Ddata.WhiteTexture = Texture3D::Create(1, 1);
		uint32_t whiteTextureData = 0xffffffff;
		s_3Ddata.WhiteTexture->SetData(&whiteTextureData, sizeof(uint32_t));

		int32_t samplers[s_RenderData.MaxTextureSlots];
		for (uint32_t i = 0; i < s_RenderData.MaxTextureSlots; i++)
			samplers[i] = i;

		s_3Ddata.cubeShader = Shader::Create("assets/shaders/Cube.glsl");
		s_3Ddata.cubeShader->Bind();

		s_3Ddata.cubeVertexPosition[0] = glm::vec4(-1.0f, -1.0f, 1.0f, 1.0f);
		s_3Ddata.cubeVertexPosition[1] = glm::vec4( 1.0f, -1.0f, 1.0f, 1.0f);
		s_3Ddata.cubeVertexPosition[2] = glm::vec4( 1.0f,  1.0f, 1.0f, 1.0f);
		s_3Ddata.cubeVertexPosition[3] = glm::vec4(-1.0f,  1.0f, 1.0f, 1.0f);

		s_3Ddata.cubeVertexPosition[4] = glm::vec4(-1.0f, -1.0f, -1.0f, 1.0f);
		s_3Ddata.cubeVertexPosition[5] = glm::vec4( 1.0f, -1.0f, -1.0f, 1.0f);
		s_3Ddata.cubeVertexPosition[6] = glm::vec4( 1.0f,  1.0f, -1.0f, 1.0f);
		s_3Ddata.cubeVertexPosition[7] = glm::vec4(-1.0f,  1.0f, -1.0f, 1.0f);

		s_3Ddata.cubeVertexPosition[8]  = glm::vec4(-1.0f, -1.0f, -1.0f, 1.0f);
		s_3Ddata.cubeVertexPosition[9]  = glm::vec4(-1.0f, -1.0f, 1.0f, 1.0f);
		s_3Ddata.cubeVertexPosition[10] = glm::vec4(-1.0f,  1.0f, 1.0f, 1.0f);
		s_3Ddata.cubeVertexPosition[11] = glm::vec4(-1.0f,  1.0f, -1.0f, 1.0f);

		s_3Ddata.cubeVertexPosition[12] = glm::vec4(1.0f, -1.0f, 1.0f, 1.0f);
		s_3Ddata.cubeVertexPosition[13] = glm::vec4(1.0f, -1.0f, -1.0f, 1.0f);
		s_3Ddata.cubeVertexPosition[14] = glm::vec4(1.0f,  1.0f, -1.0f, 1.0f);
		s_3Ddata.cubeVertexPosition[15] = glm::vec4(1.0f,  1.0f, 1.0f, 1.0f);

		s_3Ddata.cubeVertexPosition[16] = glm::vec4(-1.0f, 1.0f, 1.0f, 1.0f);
		s_3Ddata.cubeVertexPosition[17] = glm::vec4( 1.0f, 1.0f, 1.0f, 1.0f);
		s_3Ddata.cubeVertexPosition[18] = glm::vec4( 1.0f, 1.0f, -1.0f, 1.0f);
		s_3Ddata.cubeVertexPosition[19] = glm::vec4(-1.0f, 1.0f, -1.0f, 1.0f);

		s_3Ddata.cubeVertexPosition[20] = glm::vec4(-1.0f, -1.0f,  1.0f, 1.0f);
		s_3Ddata.cubeVertexPosition[21] = glm::vec4( 1.0f, -1.0f,  1.0f, 1.0f);
		s_3Ddata.cubeVertexPosition[22] = glm::vec4( 1.0f, -1.0f, -1.0f, 1.0f);
		s_3Ddata.cubeVertexPosition[23] = glm::vec4(-1.0f, -1.0f, -1.0f, 1.0f);

		
	}

	void Renderer3D::BeginScene(const Camera& camera, glm::mat4& transform)
	{
		glm::mat4 viewProjection = camera.GetProjection() * glm::inverse(transform);

		s_3Ddata.cubeShader->Bind();
		s_3Ddata.cubeShader->SetMatrix("u_ViewProjection", viewProjection);

		StartBatch();
	}

	void Renderer3D::BeginScene(const EditorCamera& camera)
	{
		s_3Ddata.cubeShader->Bind();
		s_3Ddata.cubeShader->SetMatrix("u_ViewProjection", camera.GetViewProjection());

		s_3Ddata.cubeVertexBufferPtr = s_3Ddata.cubeVertexBufferBase;

		StartBatch();
	}

	void Renderer3D::BeginScene(const glm::mat4& ViewProjection)
	{
		s_3Ddata.cubeShader->Bind();
		s_3Ddata.cubeShader->SetMatrix("u_ViewProjection", ViewProjection);

		StartBatch();
	}

	void Renderer3D::EndScene()
	{
		Flush();
	}

	void Renderer3D::Flush()
	{
		uint32_t dataSize = (uint8_t*)s_3Ddata.cubeVertexBufferPtr - (uint8_t*)s_3Ddata.cubeVertexBufferBase;
		s_3Ddata.cubeVertexBuffer->SetData(s_3Ddata.cubeVertexBufferBase, dataSize);

		/*for (uint32_t i = 0; i < s_3Ddata.TextureSlotIndex; i++)
			s_3Ddata.TextureSlots[i]->Bind(i);*/

		RenderCommand::DrawTriIndexed(s_3Ddata.cubeVertexArray,s_3Ddata.cubeVertexArray->GetIndexBuffer()->GetCount());
		
		s_3Ddata.cubeShader->Unbind();
	}

	void Renderer3D::DrawCube(const glm::mat4& transform, const glm::vec4& color)
	{
		constexpr int length = sizeof(s_3Ddata.cubeVertexPosition) / sizeof(s_3Ddata.cubeVertexPosition[0]);
		for (size_t i = 0; i < length; i++)
		{
			s_3Ddata.cubeVertexBufferPtr->Position = transform * s_3Ddata.cubeVertexPosition[i];
			s_3Ddata.cubeVertexBufferPtr->Color = color;
			s_3Ddata.cubeVertexBufferPtr++;
		}
		s_3Ddata.cubeIndexCount += 6;
	}

	void Renderer3D::Shutdown()
	{
		delete s_3Ddata.cubeVertexBufferBase;
	}

	void Renderer3D::StartBatch()
	{
		s_3Ddata.cubeIndexCount = 0;
		s_3Ddata.cubeVertexBufferPtr = s_3Ddata.cubeVertexBufferBase;
		s_3Ddata.TextureSlotIndex = 1;
	}

	void Renderer3D::NextBatch()
	{
		Flush();
		StartBatch();
	}
}