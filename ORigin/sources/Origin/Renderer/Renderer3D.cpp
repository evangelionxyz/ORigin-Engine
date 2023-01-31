// Copyright (c) 2022 Evangelion Manuhutu | ORigin Engine

#include "pch.h"
#include "Origin\Renderer\Renderer.h"
#include "Origin/Renderer/Renderer3D.h"
#include "Origin\Renderer\Renderer2D.h"

namespace Origin
{
	void Renderer3D::Init()
	{
		//  ======== Cube ========
		s_3Ddata.CubeVertexArray = VertexArray::Create();
		s_3Ddata.CubeVertexBuffer = VertexBuffer::Create(s_3Ddata.MaxVertices * sizeof(CubeVertex));
		s_3Ddata.CubeVertexBuffer->SetLayout({
			{ShaderDataType::Float3,	"a_Pos"			},
			{ShaderDataType::Float4,	"a_Color"		},
			{ShaderDataType::Float2,	"a_TexCoord"},
			{ShaderDataType::Float,		"a_TexIndex"},
			{ShaderDataType::Int,			"a_EntityID"},
		});
		
		s_3Ddata.CubeVertexArray->AddVertexBuffer(s_3Ddata.CubeVertexBuffer);
		s_3Ddata.CubeVertexBufferBase = new CubeVertex[s_3Ddata.MaxVertices];

		auto CubeIndices = new uint32_t[s_3Ddata.MaxIndices];
		uint32_t Offset = 0;
		for (uint32_t i = 0; i < s_3Ddata.MaxIndices; i += 6)
		{
			CubeIndices[i + 0] = Offset + 0;
			CubeIndices[i + 1] = Offset + 1;
			CubeIndices[i + 2] = Offset + 2;

			CubeIndices[i + 3] = Offset + 2;
			CubeIndices[i + 4] = Offset + 3;
			CubeIndices[i + 5] = Offset + 0;
			Offset += 4;
		}

		std::shared_ptr<IndexBuffer> CubeIndexBuffer = IndexBuffer::Create(CubeIndices, s_3Ddata.MaxIndices);
		s_3Ddata.CubeVertexArray->SetIndexBuffer(CubeIndexBuffer);
		delete[] CubeIndices;

		// Lighting Setup
		s_3Ddata.LightingVertexArray = VertexArray::Create();
		s_3Ddata.LightingVertexBuffer = VertexBuffer::Create(s_3Ddata.MaxVertices * sizeof(LightingVertex));
		s_3Ddata.LightingVertexBuffer->SetLayout({
			{ShaderDataType::Float3,	"a_Position"	},
			{ShaderDataType::Float4,	"a_Color"			},
			{ShaderDataType::Float,		"a_Intensity"	},
			{ShaderDataType::Int,			"a_EntityID"	},
		});

		s_3Ddata.LightingVertexArray->AddVertexBuffer(s_3Ddata.LightingVertexBuffer);
		s_3Ddata.LightingVertexBufferBase = new LightingVertex[s_3Ddata.MaxVertices];

		s_3Ddata.LightingVertexArray->SetIndexBuffer(CubeIndexBuffer);


		// Data Settings
		s_3Ddata.WhiteTexture = Texture2D::Create(1, 1);
		uint32_t whiteTextureData = 0xffffffff;
		s_3Ddata.WhiteTexture->SetData(&whiteTextureData, sizeof(uint32_t));

		int32_t samplers[s_3Ddata.MaxTextureSlots];
		for (uint32_t i = 0; i < s_3Ddata.MaxTextureSlots; i++)
			samplers[i] = i;

		s_3Ddata.TextureSlots[0] = s_3Ddata.WhiteTexture;

		// Front
		s_3Ddata.CubeVertexPosition[0]	= glm::vec4(-0.5f, -0.5f, 0.5f, 1.0f);
		s_3Ddata.CubeVertexPosition[1]	= glm::vec4( 0.5f, -0.5f, 0.5f, 1.0f);
		s_3Ddata.CubeVertexPosition[2]	= glm::vec4( 0.5f,  0.5f, 0.5f, 1.0f);
		s_3Ddata.CubeVertexPosition[3]	= glm::vec4(-0.5f,  0.5f, 0.5f, 1.0f);

		// Back
		s_3Ddata.CubeVertexPosition[4]	= glm::vec4(-0.5f, -0.5f, -0.5f, 1.0f);
		s_3Ddata.CubeVertexPosition[5]	= glm::vec4( 0.5f, -0.5f, -0.5f, 1.0f);
		s_3Ddata.CubeVertexPosition[6]	= glm::vec4( 0.5f,  0.5f, -0.5f, 1.0f);
		s_3Ddata.CubeVertexPosition[7]	= glm::vec4(-0.5f,  0.5f, -0.5f, 1.0f);

		// Left
		s_3Ddata.CubeVertexPosition[8]  = glm::vec4(-0.5f, -0.5f, -0.5f, 1.0f);
		s_3Ddata.CubeVertexPosition[9]  = glm::vec4(-0.5f, -0.5f,  0.5f, 1.0f);
		s_3Ddata.CubeVertexPosition[10] = glm::vec4(-0.5f,  0.5f,  0.5f, 1.0f);
		s_3Ddata.CubeVertexPosition[11] = glm::vec4(-0.5f,  0.5f, -0.5f, 1.0f);

		// Right
		s_3Ddata.CubeVertexPosition[12] = glm::vec4(0.5f, -0.5f,  0.5f, 1.0f);
		s_3Ddata.CubeVertexPosition[13] = glm::vec4(0.5f, -0.5f, -0.5f, 1.0f);
		s_3Ddata.CubeVertexPosition[14] = glm::vec4(0.5f,  0.5f, -0.5f, 1.0f);
		s_3Ddata.CubeVertexPosition[15] = glm::vec4(0.5f,  0.5f,  0.5f, 1.0f);

		// Top
		s_3Ddata.CubeVertexPosition[16] = glm::vec4(-0.5f, 0.5f, 0.5f, 1.0f);
		s_3Ddata.CubeVertexPosition[17] = glm::vec4( 0.5f, 0.5f, 0.5f, 1.0f);
		s_3Ddata.CubeVertexPosition[18] = glm::vec4( 0.5f, 0.5f, -0.5f, 1.0f);
		s_3Ddata.CubeVertexPosition[19] = glm::vec4(-0.5f, 0.5f, -0.5f, 1.0f);

		// Bottom
		s_3Ddata.CubeVertexPosition[20] = glm::vec4(-0.5f, -0.5f,  0.5f, 1.0f);
		s_3Ddata.CubeVertexPosition[21] = glm::vec4( 0.5f, -0.5f,  0.5f, 1.0f);
		s_3Ddata.CubeVertexPosition[22] = glm::vec4( 0.5f, -0.5f, -0.5f, 1.0f);
		s_3Ddata.CubeVertexPosition[23] = glm::vec4(-0.5f, -0.5f, -0.5f, 1.0f);

		s_3Ddata.CubeShader = Shader::Create("assets/shaders/Cube.glsl");
		s_3Ddata.LightingShader = Shader::Create("assets/shaders/Lighting.glsl");
	}

	void Renderer3D::BeginScene(const Camera& camera, const glm::mat4& transform)
	{
		StartBatch();
	}

	void Renderer3D::BeginScene(const EditorCamera& camera)
	{
		StartBatch();
	}

	void Renderer3D::StartBatch()
	{
		s_3Ddata.CubeIndexCount = 0;
		s_3Ddata.CubeVertexBufferPtr = s_3Ddata.CubeVertexBufferBase;

		s_3Ddata.LightingIndexCount = 0;
		s_3Ddata.LightingVertexBufferPtr = s_3Ddata.LightingVertexBufferBase;

		s_3Ddata.TextureSlotIndex = 1;
	}

	void Renderer3D::EndScene()
	{
		Flush();
	}

	void Renderer3D::Flush()
	{
		if(s_3Ddata.CubeIndexCount)
		{
			uint32_t dataSize = (uint8_t*)s_3Ddata.CubeVertexBufferPtr - (uint8_t*)s_3Ddata.CubeVertexBufferBase;
			s_3Ddata.CubeVertexBuffer->SetData(s_3Ddata.CubeVertexBufferBase, dataSize);

			for (uint32_t i = 0; i < s_3Ddata.TextureSlotIndex; i++)
				s_3Ddata.TextureSlots[i]->Bind(i);

			s_3Ddata.CubeShader->Bind();
			RenderCommand::DrawIndexed(s_3Ddata.CubeVertexArray, s_3Ddata.CubeIndexCount);
		}

		if (s_3Ddata.LightingIndexCount)
		{
			uint32_t dataSize = (uint8_t*)s_3Ddata.LightingVertexBufferPtr - (uint8_t*)s_3Ddata.LightingVertexBufferBase;
			s_3Ddata.LightingVertexBuffer->SetData(s_3Ddata.LightingVertexBufferBase, dataSize);

			s_3Ddata.LightingShader->Bind();
			RenderCommand::DrawIndexed(s_3Ddata.LightingVertexArray, s_3Ddata.LightingIndexCount);
		}
	}

	void Renderer3D::DrawCube(const glm::mat4& transform, SpriteRendererComponent& sprite, int entityID)
	{
		if (sprite.Texture)
			DrawCube(transform, sprite.Texture, sprite.Color, entityID);
		else
			DrawCube(transform, sprite.Color, entityID);
	}

	void Renderer3D::DrawCube(const glm::mat4& transform, const glm::vec4& color, int entityID)
	{
		constexpr uint8_t verticesSize = 24;
		const float textureIndex = 0.0f; // White Texture
		const float tilingFactor = 1.0f;
		glm::vec2 textureCoords = glm::vec2(0.0f);


		if (s_2Ddata.QuadIndexCount >= Renderer2DData::MaxIndices)
			NextBatch();

		for (size_t i = 0; i < verticesSize; i++)
		{
			s_3Ddata.CubeVertexBufferPtr->Position = transform * s_3Ddata.CubeVertexPosition[i];
			s_3Ddata.CubeVertexBufferPtr->Color = color;
			s_3Ddata.CubeVertexBufferPtr->TexCoord = textureCoords;
			s_3Ddata.CubeVertexBufferPtr->TexIndex = textureIndex;
			s_3Ddata.CubeVertexBufferPtr->EntityID = entityID;
			s_3Ddata.CubeVertexBufferPtr++;
		}
		s_3Ddata.CubeIndexCount += 36;
	}

	void Renderer3D::DrawCube(const glm::mat4& transform, const std::shared_ptr<Texture2D>& texture, const glm::vec4& tintColor, int entityID)
	{
		const uint8_t verticesSize = 24;

		constexpr glm::vec2 textureCoords[] = {
			{ 0.0f, 0.0f },
			{ 1.0f, 0.0f },
			{ 1.0f, 1.0f },
			{ 0.0f, 1.0f },

			{ 0.0f, 0.0f },
			{ 1.0f, 0.0f },
			{ 1.0f, 1.0f },
			{ 0.0f, 1.0f },

			{ 1.0f, 0.0f },
			{ 1.0f, 1.0f },
			{ 0.0f, 1.0f },
			{ 0.0f, 0.0f },

			{ 1.0f, 0.0f },
			{ 1.0f, 1.0f },
			{ 0.0f, 1.0f },
			{ 0.0f, 0.0f },

			{ 1.0f, 0.0f },
			{ 1.0f, 1.0f },
			{ 0.0f, 1.0f },
			{ 0.0f, 0.0f },

			{ 1.0f, 0.0f },
			{ 1.0f, 1.0f },
			{ 0.0f, 1.0f },
			{ 0.0f, 0.0f },
		};


		if (s_3Ddata.CubeIndexCount >= Renderer3DData::MaxIndices)
			NextBatch();

		float textureIndex = 0.0f;
		for (uint32_t i = 1; i < s_3Ddata.TextureSlotIndex; i++)
		{
			if (*s_3Ddata.TextureSlots[i] == *texture)
			{
				textureIndex = (float)i;
				break;
			}
		}

		if (textureIndex == 0.0f)
		{
			if (s_3Ddata.TextureSlotIndex >= Renderer3DData::MaxTextureSlots)
				NextBatch();

			textureIndex = (float)s_3Ddata.TextureSlotIndex;
			s_3Ddata.TextureSlots[s_3Ddata.TextureSlotIndex] = texture;
			s_3Ddata.TextureSlotIndex++;
		}

		for (size_t i = 0; i < verticesSize; i++)
		{
			s_3Ddata.CubeVertexBufferPtr->Position = transform * s_3Ddata.CubeVertexPosition[i];
			s_3Ddata.CubeVertexBufferPtr->Color = tintColor;
			s_3Ddata.CubeVertexBufferPtr->TexCoord = textureCoords[i];
			s_3Ddata.CubeVertexBufferPtr->TexIndex = textureIndex;
			s_3Ddata.CubeVertexBufferPtr->EntityID = entityID;
			s_3Ddata.CubeVertexBufferPtr++;
		}
		s_3Ddata.CubeIndexCount += 36;
	}

	void Renderer3D::DrawLight(const glm::mat4& transform, glm::vec4 color, float intensity, int entityID)
	{
		if (s_3Ddata.LightingIndexCount >= Renderer3DData::MaxIndices)
			NextBatch();

		for (auto& i : s_3Ddata.CubeVertexPosition)
		{
			s_3Ddata.LightingVertexBufferPtr->Position = transform * i;
			s_3Ddata.LightingVertexBufferPtr->Color = color;
			s_3Ddata.LightingVertexBufferPtr->Intensity = intensity;
			s_3Ddata.LightingVertexBufferPtr->EntityID = entityID;
			s_3Ddata.LightingVertexBufferPtr++;
		}
		s_3Ddata.LightingIndexCount += 36;
	}

	void Renderer3D::DrawRect(const glm::vec3& position, const glm::vec2& size, glm::vec4& color, int entityID)
	{
		glm::vec3 p0 = glm::vec3(position.x - size.x * 0.5f, position.y - size.y * 0.5f, position.z);
		glm::vec3 p1 = glm::vec3(position.x + size.x * 0.5f, position.y - size.y * 0.5f, position.z);
		glm::vec3 p2 = glm::vec3(position.x + size.x * 0.5f, position.y + size.y * 0.5f, position.z);
		glm::vec3 p3 = glm::vec3(position.x - size.x * 0.5f, position.y + size.y * 0.5f, position.z);

		Renderer2D::DrawLine(p0, p1, color);
		Renderer2D::DrawLine(p1, p2, color);
		Renderer2D::DrawLine(p2, p3, color);
		Renderer2D::DrawLine(p3, p0, color);
	}

	void Renderer3D::DrawRect(const glm::mat4& transform, glm::vec4& color, int entityID)
	{
		glm::vec3 lineVertices[24];
		for (size_t i = 0; i < 24; i++)
			lineVertices[i] = transform * s_3Ddata.CubeVertexPosition[i];

		Renderer2D::DrawLine(lineVertices[0], lineVertices[1], color);
		Renderer2D::DrawLine(lineVertices[1], lineVertices[2], color);
		Renderer2D::DrawLine(lineVertices[2], lineVertices[3], color);
		Renderer2D::DrawLine(lineVertices[3], lineVertices[0], color);

		Renderer2D::DrawLine(lineVertices[4], lineVertices[5], color);
		Renderer2D::DrawLine(lineVertices[5], lineVertices[6], color);
		Renderer2D::DrawLine(lineVertices[6], lineVertices[7], color);
		Renderer2D::DrawLine(lineVertices[7], lineVertices[4], color);

		Renderer2D::DrawLine(lineVertices[8], lineVertices[9], color);
		Renderer2D::DrawLine(lineVertices[9], lineVertices[10], color);
		Renderer2D::DrawLine(lineVertices[10], lineVertices[11], color);
		Renderer2D::DrawLine(lineVertices[11], lineVertices[8], color);

		Renderer2D::DrawLine(lineVertices[12], lineVertices[13], color);
		Renderer2D::DrawLine(lineVertices[13], lineVertices[14], color);
		Renderer2D::DrawLine(lineVertices[14], lineVertices[15], color);
		Renderer2D::DrawLine(lineVertices[15], lineVertices[12], color);

		Renderer2D::DrawLine(lineVertices[16], lineVertices[17], color);
		Renderer2D::DrawLine(lineVertices[17], lineVertices[18], color);
		Renderer2D::DrawLine(lineVertices[18], lineVertices[19], color);
		Renderer2D::DrawLine(lineVertices[19], lineVertices[16], color);

		Renderer2D::DrawLine(lineVertices[20], lineVertices[21], color);
		Renderer2D::DrawLine(lineVertices[21], lineVertices[22], color);
		Renderer2D::DrawLine(lineVertices[22], lineVertices[23], color);
		Renderer2D::DrawLine(lineVertices[23], lineVertices[20], color);
	}

	void Renderer3D::Shutdown()
	{
		delete s_3Ddata.CubeVertexBufferBase;
		delete s_3Ddata.LightingVertexBufferBase;
	}

	void Renderer3D::NextBatch()
	{
		Flush();
		StartBatch();
	}
}