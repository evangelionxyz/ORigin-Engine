// Copyright (c) Evangelion Manuhutu | ORigin Engine

#include "pch.h"
#include "Origin/Renderer/Renderer.h"
#include "Origin/Renderer/Renderer2D.h"
#include "Origin/Asset/AssetManager.h"

namespace origin {

	struct CameraBufferData
	{
		glm::mat4 ViewProjection;
		glm::vec3 Position;
	};

	struct TextVertex
	{
		glm::vec3 Position;
		glm::vec4 Color;
		glm::vec2 TexCoord;
		float TexIndex;
	};

	struct QuadVertex
	{
		glm::vec3 Position;
		glm::vec4 Color;
		glm::vec2 TexCoord;
		glm::vec2 TilingFactor;
		float TexIndex;
	};

	struct CircleVertex
	{
		glm::vec3 TransformedPosition;
		glm::vec3 Position;
		glm::vec4 Color;
		float Thickness;
		float Fade;
	};

	struct LineVertex
	{
		glm::vec3 Position;
		glm::vec4 Color;
	};

	struct Renderer2DData
	{
		// =============================================
		// =================== Texts ===================
		// =============================================

		Shader *RenderShader = nullptr;

		std::shared_ptr<VertexArray> TextVertexArray;
		std::shared_ptr<VertexBuffer> TextVertexBuffer;
		std::shared_ptr<Shader> TextShader;
		uint32_t FontAtlasTextureIndex = 0;
		std::array<std::shared_ptr<Texture2D>, 32> FontAtlasTextureSlots;
		uint32_t TextIndexCount = 0;
		TextVertex *TextVertexBufferBase = nullptr;
		TextVertex *TextVertexBufferPtr = nullptr;

		// =============================================
		// =================== Quads ===================
		// =============================================
		std::shared_ptr<VertexArray> QuadVertexArray;
		std::shared_ptr<VertexBuffer> QuadVertexBuffer;
		std::shared_ptr<Shader> QuadShader;

		uint32_t QuadIndexCount = 0;
		QuadVertex *QuadVertexBufferBase = nullptr;
		QuadVertex *QuadVertexBufferPtr = nullptr;
		glm::vec4 QuadVertexPositions[4];

		// =============================================
		// =================== Circles ===================
		// =============================================
		std::shared_ptr<VertexArray> CircleVertexArray;
		std::shared_ptr<VertexBuffer> CircleVertexBuffer;
		std::shared_ptr<Shader> CircleShader;

		uint32_t CircleIndexCount = 0;
		CircleVertex *CircleVertexBufferBase = nullptr;
		CircleVertex *CircleVertexBufferPtr = nullptr;

		// =============================================
		// =================== Lines ===================
		// =============================================
		static const uint32_t MaxLines = 102400;
		static const uint32_t MaxLineVertices = MaxLines * 2;

		std::shared_ptr<VertexArray> LineVertexArray;
		std::shared_ptr<VertexBuffer> LineVertexBuffer;
		std::shared_ptr<Shader> LineShader;

		uint32_t LineVertexCount = 0;
		LineVertex *LineVertexBufferBase = nullptr;
		LineVertex *LineVertexBufferPtr = nullptr;

		std::array<std::shared_ptr<Texture2D>, 32> TextureSlots;
		uint32_t TextureSlotIndex = 1; // 0 = white texture
	};

	static Renderer2DData s_Render2DData;
	static CameraBufferData s_CameraBufferData;
	static std::shared_ptr<UniformBuffer> s_CameraUniformBuffer;

	void Renderer2D::Init()
	{
		OGN_PROFILER_FUNCTION();

		s_CameraUniformBuffer = UniformBuffer::Create(sizeof(CameraBufferData), CAMERA_BINDING);

		// Quads
		s_Render2DData.QuadVertexArray = VertexArray::Create();

		s_Render2DData.QuadVertexBuffer = VertexBuffer::Create(Renderer::s_RenderData.MaxVertices * sizeof(QuadVertex));
		s_Render2DData.QuadVertexBuffer->SetLayout({
			{ ShaderDataType::Float3, "aPosition"     },
			{ ShaderDataType::Float4, "aColor"        },
			{ ShaderDataType::Float2, "aTexCoord"     },
			{ ShaderDataType::Float2, "aTilingFactor" },
			{ ShaderDataType::Float,  "aTexIndex"     },
			});

		s_Render2DData.QuadVertexArray->AddVertexBuffer(s_Render2DData.QuadVertexBuffer);
		s_Render2DData.QuadVertexBufferBase = new QuadVertex[Renderer::s_RenderData.MaxVertices];

		uint32_t* quadIndices = new uint32_t[Renderer::s_RenderData.MaxQuadIndices];

		uint32_t offset = 0;
		for (uint32_t i = 0; i < Renderer::s_RenderData.MaxQuadIndices; i += 6)
		{
			quadIndices[i + 0] = offset + 0;
			quadIndices[i + 1] = offset + 1;
			quadIndices[i + 2] = offset + 2;

			quadIndices[i + 3] = offset + 2;
			quadIndices[i + 4] = offset + 3;
			quadIndices[i + 5] = offset + 0;

			offset += 4;
		}

		std::shared_ptr<IndexBuffer> quadIB = IndexBuffer::Create(quadIndices, Renderer::s_RenderData.MaxQuadIndices);
		s_Render2DData.QuadVertexArray->SetIndexBuffer(quadIB);
		delete[] quadIndices;

		// Text
		s_Render2DData.TextVertexArray = VertexArray::Create();
		s_Render2DData.TextVertexBuffer = VertexBuffer::Create(Renderer::s_RenderData.MaxVertices * sizeof(TextVertex));

		s_Render2DData.TextVertexBuffer->SetLayout({
			{ ShaderDataType::Float3, "aPosition" },
			{ ShaderDataType::Float4, "aColor"    },
			{ ShaderDataType::Float2, "aTexCoord" },
			{ ShaderDataType::Float,  "aTexIndex" },
			});

		s_Render2DData.TextVertexArray->AddVertexBuffer(s_Render2DData.TextVertexBuffer);
		s_Render2DData.TextVertexArray->SetIndexBuffer(quadIB);
		s_Render2DData.TextVertexBufferBase = new TextVertex[Renderer::s_RenderData.MaxVertices];

		// Circles
		s_Render2DData.CircleVertexArray = VertexArray::Create();
		s_Render2DData.CircleVertexBuffer = VertexBuffer::Create(Renderer::s_RenderData.MaxVertices * sizeof(CircleVertex));
		s_Render2DData.CircleVertexBuffer->SetLayout({
			{ ShaderDataType::Float3, "aTransformedPosition"  },
			{ ShaderDataType::Float3, "aPosition"  },
			{ ShaderDataType::Float4, "aColor"     },
			{ ShaderDataType::Float,  "aThickness" },
			{ ShaderDataType::Float,  "aFade"      },
			});
		s_Render2DData.CircleVertexArray->AddVertexBuffer(s_Render2DData.CircleVertexBuffer);
		s_Render2DData.CircleVertexArray->SetIndexBuffer(quadIB);
		s_Render2DData.CircleVertexBufferBase = new CircleVertex[Renderer::s_RenderData.MaxVertices];

		// Lines
		s_Render2DData.LineVertexArray = VertexArray::Create();
		s_Render2DData.LineVertexBuffer = VertexBuffer::Create(Renderer::s_RenderData.MaxVertices * sizeof(LineVertex));
		s_Render2DData.LineVertexBuffer->SetLayout({
			{ ShaderDataType::Float3, "aPosition" },
			{ ShaderDataType::Float4, "aColor"    },
			});

		s_Render2DData.LineVertexArray->AddVertexBuffer(s_Render2DData.LineVertexBuffer);
		s_Render2DData.LineVertexBufferBase = new LineVertex[Renderer::s_RenderData.MaxVertices];

		// Set first texture slot to 0
		s_Render2DData.TextureSlots[0] = Renderer::WhiteTexture;

		s_Render2DData.QuadVertexPositions[0] = { -0.5f, -0.5f, 0.0f, 1.0f };
		s_Render2DData.QuadVertexPositions[1] = {  0.5f, -0.5f, 0.0f, 1.0f };
		s_Render2DData.QuadVertexPositions[2] = {  0.5f,  0.5f, 0.0f, 1.0f };
		s_Render2DData.QuadVertexPositions[3] = { -0.5f,  0.5f, 0.0f, 1.0f };

		s_Render2DData.QuadShader = Renderer::GetShader("Quad2D");
		s_Render2DData.TextShader = Renderer::GetShader("Text");
		s_Render2DData.CircleShader = Renderer::GetShader("Circle2D");
		s_Render2DData.LineShader = Renderer::GetShader("Line2D");
	}

	void Renderer2D::Shutdown()
	{
		delete[] s_Render2DData.QuadVertexBufferBase;
	}

	void Renderer2D::Begin(const Camera &camera, Shader *renderShader)
	{
		s_CameraBufferData.ViewProjection = camera.GetViewProjection();
		s_CameraBufferData.Position = camera.GetPosition();
		s_CameraUniformBuffer->Bind();
		s_CameraUniformBuffer->SetData(&s_CameraBufferData, sizeof(CameraBufferData));

		s_Render2DData.RenderShader = renderShader;

		StartBatch();
	}

    void Renderer2D::Begin(const glm::mat4 &viewProjection, glm::vec3 pos)
    {
		s_CameraBufferData.ViewProjection = viewProjection;
		s_CameraBufferData.Position = pos;
        s_CameraUniformBuffer->Bind();
        s_CameraUniformBuffer->SetData(&s_CameraBufferData, sizeof(CameraBufferData));

        StartBatch();
    }

    void Renderer2D::End()
	{
		Flush();
	}

	void Renderer2D::StartBatch()
	{
		OGN_PROFILER_RENDERING();

		s_Render2DData.QuadIndexCount = 0;
		s_Render2DData.QuadVertexBufferPtr = s_Render2DData.QuadVertexBufferBase;

		s_Render2DData.TextIndexCount = 0;
		s_Render2DData.TextVertexBufferPtr = s_Render2DData.TextVertexBufferBase;

		s_Render2DData.CircleIndexCount = 0;
		s_Render2DData.CircleVertexBufferPtr = s_Render2DData.CircleVertexBufferBase;

		s_Render2DData.LineVertexCount = 0;
		s_Render2DData.LineVertexBufferPtr = s_Render2DData.LineVertexBufferBase;

		s_Render2DData.TextureSlotIndex = 1;
		s_Render2DData.FontAtlasTextureIndex = 0;
	}

	void Renderer2D::Flush()
	{
		OGN_PROFILER_RENDERING();

		if (s_Render2DData.TextIndexCount)
		{
			uint32_t dataSize = (uint32_t)((uint8_t*)s_Render2DData.TextVertexBufferPtr - (uint8_t*)s_Render2DData.TextVertexBufferBase);
			s_Render2DData.TextVertexBuffer->Bind();
			s_Render2DData.TextVertexBuffer->SetData(s_Render2DData.TextVertexBufferBase, dataSize);

			for (uint32_t i = 0; i < s_Render2DData.FontAtlasTextureIndex; i++)
			{
				s_Render2DData.FontAtlasTextureSlots[i]->Bind(i);
			}

			if (!s_Render2DData.RenderShader)
			{
				s_Render2DData.TextShader->Enable();
			}

			RenderCommand::DrawIndexed(s_Render2DData.TextVertexArray, s_Render2DData.TextIndexCount);
			Renderer::GetStatistics().DrawCalls++;
		}

		if (s_Render2DData.CircleIndexCount)
		{
			uint32_t dataSize = (uint32_t)((uint8_t*)s_Render2DData.CircleVertexBufferPtr - (uint8_t*)s_Render2DData.CircleVertexBufferBase);
			s_Render2DData.CircleVertexBuffer->Bind();
			s_Render2DData.CircleVertexBuffer->SetData(s_Render2DData.CircleVertexBufferBase, dataSize);

			if (!s_Render2DData.RenderShader)
			{
				s_Render2DData.CircleShader->Enable();
			}

			RenderCommand::DrawIndexed(s_Render2DData.CircleVertexArray, s_Render2DData.CircleIndexCount);
			Renderer::GetStatistics().DrawCalls++;
		}

		if (s_Render2DData.QuadIndexCount)
		{
			uint32_t dataSize = (uint32_t)((uint8_t *)s_Render2DData.QuadVertexBufferPtr - (uint8_t *)s_Render2DData.QuadVertexBufferBase);
			s_Render2DData.QuadVertexBuffer->Bind();
			s_Render2DData.QuadVertexBuffer->SetData(s_Render2DData.QuadVertexBufferBase, dataSize);

			for (uint32_t i = 0; i < s_Render2DData.TextureSlotIndex; i++)
			{
				s_Render2DData.TextureSlots[i]->Bind(i);
			}

			if (!s_Render2DData.RenderShader)
			{
				s_Render2DData.QuadShader->Enable();
			}

			RenderCommand::DrawIndexed(s_Render2DData.QuadVertexArray, s_Render2DData.QuadIndexCount);
			Renderer::GetStatistics().DrawCalls++;
		}

		if (s_Render2DData.LineVertexCount)
		{
			uint32_t dataSize = (uint32_t)((uint8_t *)s_Render2DData.LineVertexBufferPtr - (uint8_t *)s_Render2DData.LineVertexBufferBase);
			s_Render2DData.LineVertexBuffer->Bind();
			s_Render2DData.LineVertexBuffer->SetData(s_Render2DData.LineVertexBufferBase, dataSize);

			if (!s_Render2DData.RenderShader)
			{
				s_Render2DData.LineShader->Enable();
			}

			RenderCommand::DrawLines(s_Render2DData.LineVertexArray, s_Render2DData.LineVertexCount);
			Renderer::GetStatistics().DrawCalls++;
		}
	}

	void Renderer2D::NextBatch()
	{
		Flush();
		StartBatch();
	}

	void Renderer2D::DrawQuad(const glm::vec2& position, const glm::vec2& size, const glm::vec4& color)
	{
		DrawQuad({ position.x, position.y, 0.0f }, size, color);
	}

	void Renderer2D::DrawQuad(const glm::vec3& position, const glm::vec2& size, const glm::vec4& color)
	{
		glm::mat4 transform = glm::translate(glm::mat4(1.0f), position)
			* glm::scale(glm::mat4(1.0f), { size.x, size.y, 1.0f });

		DrawQuad(transform, color);
	}

	void Renderer2D::DrawQuad(const glm::vec2& position, const glm::vec2& size, const std::shared_ptr<Texture2D>& texture, const glm::vec2& tilingFactor, const glm::vec4& tintColor)
	{
		DrawQuad({ position.x, position.y, 0.0f }, size, texture, tilingFactor, tintColor);
	}

	void Renderer2D::DrawQuad(const glm::vec3& position, const glm::vec2& size, const std::shared_ptr<Texture2D>& texture, const glm::vec2& tilingFactor, const glm::vec4& tintColor)
	{
		glm::mat4 transform = glm::translate(glm::mat4(1.0f), position)
			* glm::scale(glm::mat4(1.0f), { size.x, size.y, 1.0f });

		DrawQuad(transform, texture, tilingFactor, tintColor);
	}

	void Renderer2D::DrawQuad(const glm::mat4& transform, const glm::vec4& color)
	{
		OGN_PROFILER_RENDERING();

		constexpr size_t quadVertexCount = 4;
		const float textureIndex = 0.0f; // White Texture
		constexpr glm::vec2 textureCoords[] = { { 0.0f, 0.0f }, { 1.0f, 0.0f }, { 1.0f, 1.0f }, { 0.0f, 1.0f } };
		const glm::vec2& tilingFactor = glm::vec2(1.0f);

		if (s_Render2DData.QuadIndexCount >= Renderer::s_RenderData.MaxQuadIndices)
			NextBatch();

		for (size_t i = 0; i < quadVertexCount; i++)
		{
			s_Render2DData.QuadVertexBufferPtr->Position = transform * s_Render2DData.QuadVertexPositions[i];
			s_Render2DData.QuadVertexBufferPtr->Color = color;
			s_Render2DData.QuadVertexBufferPtr->TexCoord = textureCoords[i];
			s_Render2DData.QuadVertexBufferPtr->TexIndex = textureIndex;
			s_Render2DData.QuadVertexBufferPtr->TilingFactor = tilingFactor;
			s_Render2DData.QuadVertexBufferPtr++;
		}

		s_Render2DData.QuadIndexCount += 6;
		Renderer::GetStatistics().QuadCount++;
	}

	void Renderer2D::DrawQuad(const glm::mat4& transform, const std::shared_ptr<Texture2D>& texture, const glm::vec2& tilingFactor, const glm::vec4& tintColor)
	{
		OGN_PROFILER_RENDERING();

		OGN_CORE_ASSERT(texture, "Renderer2D: Invalid texture");

		constexpr size_t quadVertexCount = 4;
		constexpr glm::vec2 textureCoords[] = { { 0.0f, 0.0f }, { 1.0f, 0.0f }, { 1.0f, 1.0f }, { 0.0f, 1.0f } };

		if (s_Render2DData.QuadIndexCount >= Renderer::s_RenderData.MaxQuadIndices)
			NextBatch();

		float textureIndex = 0.0f;
		for (uint32_t i = 1; i < s_Render2DData.TextureSlotIndex; i++)
		{
			if (*s_Render2DData.TextureSlots[i] == *texture)
			{
				textureIndex = (float)i;
				break;
			}
		}

		if (textureIndex == 0.0f)
		{
			if (s_Render2DData.TextureSlotIndex >= Renderer::s_RenderData.MaxTextureSlots)
				NextBatch();

			textureIndex = (float)s_Render2DData.TextureSlotIndex;
			s_Render2DData.TextureSlots[s_Render2DData.TextureSlotIndex] = texture;
			s_Render2DData.TextureSlotIndex++;
		}

		for (size_t i = 0; i < quadVertexCount; i++)
		{
			s_Render2DData.QuadVertexBufferPtr->Position = transform * s_Render2DData.QuadVertexPositions[i];
			s_Render2DData.QuadVertexBufferPtr->Color = tintColor;
			s_Render2DData.QuadVertexBufferPtr->TexCoord = textureCoords[i];
			s_Render2DData.QuadVertexBufferPtr->TexIndex = textureIndex;
			s_Render2DData.QuadVertexBufferPtr->TilingFactor = tilingFactor;
			s_Render2DData.QuadVertexBufferPtr++;
		}
		s_Render2DData.QuadIndexCount += 6;
		Renderer::GetStatistics().QuadCount++;
	}

	void Renderer2D::DrawQuad(const glm::mat4& transform, const std::shared_ptr<SubTexture2D>& subTexture, const glm::vec2& tilingFactor, const glm::vec4& tintColor)
	{
		OGN_PROFILER_RENDERING();

		constexpr int QuadVertexCount = 4;
		const glm::vec2* textureCoords = subTexture->GetTexCoords();
		const std::shared_ptr<Texture2D> texture = subTexture->GetTexture();

		if (s_Render2DData.QuadIndexCount >= Renderer::s_RenderData.MaxQuadIndices)
			NextBatch();

		float textureIndex = 0.0f;
		for (uint32_t i = 1; i < s_Render2DData.TextureSlotIndex; i++)
		{
			if (*s_Render2DData.TextureSlots[i] == *texture)
			{
				textureIndex = (float)i;
				break;
			}
		}

		if (textureIndex == 0.0f)
		{
			if (s_Render2DData.TextureSlotIndex >= Renderer::s_RenderData.MaxTextureSlots)
				NextBatch();

			textureIndex = (float)s_Render2DData.TextureSlotIndex;
			s_Render2DData.TextureSlots[s_Render2DData.TextureSlotIndex] = texture;
			s_Render2DData.TextureSlotIndex++;
		}

		for (size_t i = 0; i < QuadVertexCount; i++)
		{
			s_Render2DData.QuadVertexBufferPtr->Position = transform * s_Render2DData.QuadVertexPositions[i];
			s_Render2DData.QuadVertexBufferPtr->Color = tintColor;
			s_Render2DData.QuadVertexBufferPtr->TexCoord = textureCoords[i];
			s_Render2DData.QuadVertexBufferPtr->TexIndex = textureIndex;
			s_Render2DData.QuadVertexBufferPtr->TilingFactor = tilingFactor;
			s_Render2DData.QuadVertexBufferPtr++;
		}
		s_Render2DData.QuadIndexCount += 6;
		Renderer::GetStatistics().QuadCount++;
	}

	void Renderer2D::DrawRotatedQuad(const glm::vec2 &position, const glm::vec2 &size, float rotation, const glm::vec4 &color)
	{
		DrawRotatedQuad({ position.x, position.y, 0.0f }, size, rotation, color);
	}

	void Renderer2D::DrawRotatedQuad(const glm::vec3& position, const glm::vec2& size, float rotation, const glm::vec4& color)
	{
		glm::mat4 transform = glm::translate(glm::mat4(1.0f), position)
			* glm::rotate(glm::mat4(1.0f), rotation, { 0.0f, 0.0f, 1.0f })
			* glm::scale(glm::mat4(1.0f), { size.x, size.y, 1.0f });

		DrawQuad(transform, color);
	}

	void Renderer2D::DrawRotatedQuad(const glm::vec2& position, const glm::vec2& size, float rotation, const std::shared_ptr<Texture2D>& texture, const glm::vec2& tilingFactor, const glm::vec4& tintColor)
	{
		DrawRotatedQuad({ position.x, position.y, 0.0f }, size, rotation, texture, tilingFactor, tintColor);
	}

	void Renderer2D::DrawRotatedQuad(const glm::vec3& position, const glm::vec2& size, float rotation, const std::shared_ptr<Texture2D>& texture, const glm::vec2& tilingFactor, const glm::vec4& tintColor)
	{
		glm::mat4 transform = glm::translate(glm::mat4(1.0f), position)
			* glm::rotate(glm::mat4(1.0f), rotation, { 0.0f, 0.0f, 1.0f })
			* glm::scale(glm::mat4(1.0f), { size.x, size.y, 1.0f });

		DrawQuad(transform, texture, tilingFactor, tintColor);
	}

	void Renderer2D::DrawRect(const glm::vec3& position, const glm::vec2& size, const glm::vec4& color)
	{
		glm::vec3 p0 = glm::vec3(position.x - size.x * 0.5f, position.y - size.y * 0.5f, position.z);
		glm::vec3 p1 = glm::vec3(position.x + size.x * 0.5f, position.y - size.y * 0.5f, position.z);
		glm::vec3 p2 = glm::vec3(position.x + size.x * 0.5f, position.y + size.y * 0.5f, position.z);
		glm::vec3 p3 = glm::vec3(position.x - size.x * 0.5f, position.y + size.y * 0.5f, position.z);

		DrawLine(p0, p1, color);
		DrawLine(p1, p2, color);
		DrawLine(p2, p3, color);
		DrawLine(p3, p0, color);
	}

	void Renderer2D::DrawRect(const glm::mat4& transform, const glm::vec4& color)
	{
		glm::vec3 lineVertices[4];

		for (size_t i = 0; i < 4; i++)
			lineVertices[i] = transform * s_Render2DData.QuadVertexPositions[i];

		DrawLine(lineVertices[0], lineVertices[1], color);
		DrawLine(lineVertices[1], lineVertices[2], color);
		DrawLine(lineVertices[2], lineVertices[3], color);
		DrawLine(lineVertices[3], lineVertices[0], color);
	}

	void Renderer2D::DrawCircle(const glm::mat4& transform, const glm::vec4& color, float thickness, float fade)
	{
		OGN_PROFILER_RENDERING();

		for (size_t i = 0; i < 4; i++)
		{
			s_Render2DData.CircleVertexBufferPtr->TransformedPosition = transform * s_Render2DData.QuadVertexPositions[i];
			s_Render2DData.CircleVertexBufferPtr->Position = s_Render2DData.QuadVertexPositions[i];
			s_Render2DData.CircleVertexBufferPtr->Color = color;
			s_Render2DData.CircleVertexBufferPtr->Thickness = thickness;
			s_Render2DData.CircleVertexBufferPtr->Fade = fade;
			s_Render2DData.CircleVertexBufferPtr++;
		}

		s_Render2DData.CircleIndexCount += 6;
		Renderer::GetStatistics().CircleCount++;
	}

	void Renderer2D::DrawCircle(const glm::vec3 &position, const glm::vec2 &scale, const glm::vec4 &color, float thickness, float fade)
	{
		const glm::mat4 transform = glm::translate(glm::mat4(1.0f), position) * glm::scale(glm::mat4(1.0f), { scale.x, scale.y, 1.0f });
		DrawCircle(transform, color, thickness, fade);
	}

	void Renderer2D::DrawLine(const glm::vec3 &p0, const glm::vec3 &p1, const glm::vec4 &color)
	{
		OGN_PROFILER_RENDERING();

		if (s_Render2DData.LineVertexCount >= Renderer2DData::MaxLines)
			NextBatch();

		s_Render2DData.LineVertexBufferPtr->Position = p0;
		s_Render2DData.LineVertexBufferPtr->Color = color;
		s_Render2DData.LineVertexBufferPtr++;

		s_Render2DData.LineVertexBufferPtr->Position = p1;
		s_Render2DData.LineVertexBufferPtr->Color = color;
		s_Render2DData.LineVertexBufferPtr++;

		s_Render2DData.LineVertexCount += 2;
		Renderer::GetStatistics().LineCount++;
	}

	void Renderer2D::DrawSprite(const glm::mat4& transform, SpriteRenderer2DComponent& src)
	{
		OGN_PROFILER_RENDERING();

		const std::shared_ptr<Texture2D> &texture = AssetManager::GetAsset<Texture2D>(src.Texture);
		if (texture)
		{
			constexpr size_t quadVertexCount = 4;
			glm::vec2 textureCoords[4]
			{
				{ src.Min.x, src.Min.y },
				{ src.Max.x, src.Min.y },
				{ src.Max.x, src.Max.y },
				{ src.Min.x, src.Max.y }
			};

			if (src.FlipX)
			{
				textureCoords[0] = { src.Max.x, src.Min.y };
				textureCoords[1] = { src.Min.x, src.Min.y };
				textureCoords[2] = { src.Min.x, src.Max.y };
				textureCoords[3] = { src.Max.x, src.Max.y };
			}
			else if (src.FlipY)
			{
				textureCoords[0] = { src.Min.x, src.Max.y };
				textureCoords[1] = { src.Max.x, src.Max.y };
				textureCoords[2] = { src.Max.x, src.Min.y };
				textureCoords[3] = { src.Min.x, src.Min.y };
			}

			if (src.FlipX && src.FlipY)
			{
				textureCoords[0] = { src.Max.x, src.Max.y };
				textureCoords[1] = { src.Min.x, src.Max.y };
				textureCoords[2] = { src.Min.x, src.Min.y };
				textureCoords[3] = { src.Max.x, src.Min.y };
			}

			if (s_Render2DData.QuadIndexCount >= Renderer::s_RenderData.MaxQuadIndices)
				NextBatch();

			float textureIndex = 0.0f;
			for (uint32_t i = 1; i < s_Render2DData.TextureSlotIndex; i++)
			{
				if (*s_Render2DData.TextureSlots[i] == *texture)
				{
					textureIndex = (float)i;
					break;
				}
			}

			if (textureIndex == 0.0f)
			{
				if (s_Render2DData.TextureSlotIndex >= Renderer::s_RenderData.MaxTextureSlots)
					NextBatch();

				textureIndex = (float)s_Render2DData.TextureSlotIndex;
				s_Render2DData.TextureSlots[s_Render2DData.TextureSlotIndex] = texture;
				s_Render2DData.TextureSlotIndex++;
			}

			for (size_t i = 0; i < quadVertexCount; i++)
			{
				s_Render2DData.QuadVertexBufferPtr->Position = transform * s_Render2DData.QuadVertexPositions[i];
				s_Render2DData.QuadVertexBufferPtr->Color = src.Color;
				s_Render2DData.QuadVertexBufferPtr->TexCoord = textureCoords[i];
				s_Render2DData.QuadVertexBufferPtr->TexIndex = textureIndex;
				s_Render2DData.QuadVertexBufferPtr->TilingFactor = src.TillingFactor;
				s_Render2DData.QuadVertexBufferPtr++;
			}
			s_Render2DData.QuadIndexCount += 6;
			Renderer::GetStatistics().QuadCount++;
		}
		else
		{
			DrawQuad(transform, src.Color);
		}
	}

	void Renderer2D::DrawString(const std::string& string, std::shared_ptr<Font> font, const glm::mat4& transform, const TextParams& textParams, glm::vec2 *size)
	{
		OGN_PROFILER_RENDERING();

		if (!font)
			return;

		if (!font->IsLoaded || font->GetAtlasTexture() == nullptr)
			return;

		if (s_Render2DData.FontAtlasTextureIndex >= Renderer::s_RenderData.MaxTextureSlots)
			NextBatch();
		
		const auto& fontGeometry = font->GetData()->FontGeometry;
		const auto& metrics = fontGeometry.getMetrics();

		std::shared_ptr<Texture2D> fontAtlas = font->GetAtlasTexture();

		float textureIndex = 0;
		for (uint32_t i = 0; i < s_Render2DData.FontAtlasTextureIndex; i++)
		{
			if (*s_Render2DData.FontAtlasTextureSlots[i] == *fontAtlas)
			{
				s_Render2DData.FontAtlasTextureSlots[i] = fontAtlas;
				textureIndex = (float)i;
				break;
			}
		}

		if(textureIndex == 0.0f)
		{
			if (s_Render2DData.FontAtlasTextureIndex >= Renderer::s_RenderData.MaxTextureSlots)
				NextBatch();

			textureIndex = (float)s_Render2DData.FontAtlasTextureIndex;
			s_Render2DData.FontAtlasTextureSlots[s_Render2DData.FontAtlasTextureIndex] = fontAtlas;
			s_Render2DData.FontAtlasTextureIndex++;
		}
		
		double x = 0.0;
		double y = 0.0;
		double maxX = 0.0; // to track the maximum width
		double minY = 0.0; // to track the minimum y position (since y dreases with line breaks)
		double fsScale = 1.0 / (metrics.ascenderY - metrics.descenderY);
		const float spaceGlyphAdvance = fontGeometry.getGlyph(' ')->getAdvance();

		for (size_t i = 0; i < string.size(); i++)
		{
			char character = string[i];
			if (character == '\r')
			{
				continue;
			}

			if (character == '\n')
			{
				maxX = std::max(maxX, x);

				x = 0.0;
				y -= fsScale * metrics.lineHeight + textParams.LineSpacing;
				continue;
			}

			if (character == ' ')
			{
				float advance = spaceGlyphAdvance;
				if (i < string.size() - 1)
				{
					char nextCharacter = string[i + 1];
					double dAdvance;
					fontGeometry.getAdvance(dAdvance, character, nextCharacter);
					advance = (float)advance;
				}

				x += fsScale * advance + textParams.Kerning;
				continue;
			}

			if (character == '\t')
			{
				x += 4.0f * (fsScale * spaceGlyphAdvance + textParams.Kerning);
				continue;
			}

			auto glyph = fontGeometry.getGlyph(character);
			if (!glyph)
			{
				glyph = fontGeometry.getGlyph('?');
			}

			double atlasLeft, atlasBottom, atlasRight, atlasTop;
			glyph->getQuadAtlasBounds(atlasLeft, atlasBottom, atlasRight, atlasTop);
			glm::vec2 texCoordMin(float(atlasLeft), (float)atlasBottom);
			glm::vec2 texCoordMax(float(atlasRight), (float) atlasTop);

			double planeLeft, planeBottom, planeRight, planeTop;
			glyph->getQuadPlaneBounds(planeLeft, planeBottom, planeRight, planeTop);
			glm::vec2 quadMin((float)planeLeft, (float)planeBottom);
			glm::vec2 quadMax((float)planeRight, (float)planeTop);

			quadMin *= fsScale, quadMax *= fsScale;
			quadMin += glm::vec2(x, y);
			quadMax += glm::vec2(x, y);

			float texelWidth = 1.0f / s_Render2DData.FontAtlasTextureSlots[(int)textureIndex]->GetWidth();
			float texelHeight = 1.0f / s_Render2DData.FontAtlasTextureSlots[(int)textureIndex]->GetHeight();

			texCoordMin *= glm::vec2(texelWidth, texelHeight);
			texCoordMax *= glm::vec2(texelWidth, texelHeight);

			{
				s_Render2DData.TextVertexBufferPtr->Position = transform * glm::vec4(quadMin, 0.0f, 1.0f);
				s_Render2DData.TextVertexBufferPtr->Color = textParams.Color;
				s_Render2DData.TextVertexBufferPtr->TexCoord = texCoordMin;
				s_Render2DData.TextVertexBufferPtr->TexIndex = textureIndex;
				s_Render2DData.TextVertexBufferPtr++;

				s_Render2DData.TextVertexBufferPtr->Position = transform * glm::vec4(quadMin.x, quadMax.y, 0.0f, 1.0f);
				s_Render2DData.TextVertexBufferPtr->Color = textParams.Color;
				s_Render2DData.TextVertexBufferPtr->TexCoord = { texCoordMin.x, texCoordMax.y };
				s_Render2DData.TextVertexBufferPtr->TexIndex = textureIndex;
				s_Render2DData.TextVertexBufferPtr++;

				s_Render2DData.TextVertexBufferPtr->Position = transform * glm::vec4(quadMax, 0.0f, 1.0f);
				s_Render2DData.TextVertexBufferPtr->Color = textParams.Color;
				s_Render2DData.TextVertexBufferPtr->TexCoord = texCoordMax;
				s_Render2DData.TextVertexBufferPtr->TexIndex = textureIndex;
				s_Render2DData.TextVertexBufferPtr++;

				s_Render2DData.TextVertexBufferPtr->Position = transform * glm::vec4(quadMax.x, quadMin.y, 0.0f, 1.0f);
				s_Render2DData.TextVertexBufferPtr->Color = textParams.Color;
				s_Render2DData.TextVertexBufferPtr->TexCoord = { texCoordMax.x, texCoordMin.y };
				s_Render2DData.TextVertexBufferPtr->TexIndex = textureIndex;
				s_Render2DData.TextVertexBufferPtr++;

				s_Render2DData.TextIndexCount += 6;
				Renderer::GetStatistics().QuadCount++;
			}

			if (i < string.size() - 1)
			{
				double advance = glyph->getAdvance();
				char nextCharacter = string[i + 1];
				fontGeometry.getAdvance(advance, character, nextCharacter);
				x += fsScale * advance + textParams.Kerning;
			}

			maxX = std::max(maxX, x);
			minY = std::min(minY, y);
		}

		if (size)
		{
			double totalWidth = maxX;
			double totalHeight = std::abs(minY) + fsScale * metrics.lineHeight;
			*size = { static_cast<float>(maxX), static_cast<float>(totalHeight) };
		}
	}

	void Renderer2D::DrawString(const std::string& string, const glm::mat4& transform, TextComponent& component)
	{
		std::shared_ptr<Font> font = AssetManager::GetAsset<Font>(component.FontHandle);
		DrawString(string, font, transform, { component.Color, component.Kerning, component.LineSpacing }, &component.Size);
	}
}