// Copyright (c) Evangelion Manuhutu | ORigin Engine

#include "pch.h"
#include "Origin\Renderer\Renderer.h"
#include "Origin\Renderer\Renderer2D.h"

#include "MSDFData.h"

#include "Origin\Asset\AssetManager.h"

namespace origin {

	void Renderer2D::Init()
	{
		// Quads
		s_2Ddata.QuadVertexArray = VertexArray::Create();

		s_2Ddata.QuadVertexBuffer = VertexBuffer::Create(s_2Ddata.MaxVertices * sizeof(QuadVertex));
		s_2Ddata.QuadVertexBuffer->SetLayout({
			{ ShaderDataType::Float3, "aPosition"     },
			{ ShaderDataType::Float4, "aColor"        },
			{ ShaderDataType::Float2, "aTexCoord"     },
			{ ShaderDataType::Float,  "aTexIndex"     },
			{ ShaderDataType::Float2, "aTilingFactor" },
			{ ShaderDataType::Int,    "aEntityID"     }
			});
		s_2Ddata.QuadVertexArray->AddVertexBuffer(s_2Ddata.QuadVertexBuffer);
		s_2Ddata.QuadVertexBufferBase = new QuadVertex[s_2Ddata.MaxVertices];

		uint32_t* quadIndices = new uint32_t[s_2Ddata.MaxIndices];

		uint32_t offset = 0;
		for (uint32_t i = 0; i < s_2Ddata.MaxIndices; i += 6)
		{
			quadIndices[i + 0] = offset + 0;
			quadIndices[i + 1] = offset + 1;
			quadIndices[i + 2] = offset + 2;

			quadIndices[i + 3] = offset + 2;
			quadIndices[i + 4] = offset + 3;
			quadIndices[i + 5] = offset + 0;

			offset += 4;
		}

		std::shared_ptr<IndexBuffer> quadIB = IndexBuffer::Create(quadIndices, s_2Ddata.MaxIndices);
		s_2Ddata.QuadVertexArray->SetIndexBuffer(quadIB);
		delete[] quadIndices;

		// Text
		s_2Ddata.TextVertexArray = VertexArray::Create();
		s_2Ddata.TextVertexBuffer = VertexBuffer::Create(s_2Ddata.MaxVertices * sizeof(TextVertex));

		s_2Ddata.TextVertexBuffer->SetLayout({
			{ ShaderDataType::Float3, "a_Position"     },
			{ ShaderDataType::Float4, "a_Color"        },
			{ ShaderDataType::Float2, "a_TexCoord"     },
			{ ShaderDataType::Float, "a_TexIndex"     },
			{ ShaderDataType::Int,    "a_EntityID"     }
			});

		s_2Ddata.TextVertexArray->AddVertexBuffer(s_2Ddata.TextVertexBuffer);
		s_2Ddata.TextVertexArray->SetIndexBuffer(quadIB);
		s_2Ddata.TextVertexBufferBase = new TextVertex[s_2Ddata.MaxVertices];

		// Circles
		s_2Ddata.CircleVertexArray = VertexArray::Create();
		s_2Ddata.CircleVertexBuffer = VertexBuffer::Create(s_2Ddata.MaxVertices * sizeof(CircleVertex));
		s_2Ddata.CircleVertexBuffer->SetLayout({
			{ ShaderDataType::Float3, "aWorldPosition" },
			{ ShaderDataType::Float3, "aLocalPosition" },
			{ ShaderDataType::Float4, "aColor"					},
			{ ShaderDataType::Float,  "aThickness"			},
			{ ShaderDataType::Float,  "aFade"					},
			{ ShaderDataType::Int,    "aEntityID"			}
			});
		s_2Ddata.CircleVertexArray->AddVertexBuffer(s_2Ddata.CircleVertexBuffer);
		s_2Ddata.CircleVertexArray->SetIndexBuffer(quadIB);
		s_2Ddata.CircleVertexBufferBase = new CircleVertex[s_2Ddata.MaxVertices];

		// Lines
		s_2Ddata.LineVertexArray = VertexArray::Create();
		s_2Ddata.LineVertexBuffer = VertexBuffer::Create(s_2Ddata.MaxVertices * sizeof(LineVertex));
		s_2Ddata.LineVertexBuffer->SetLayout({
			{ ShaderDataType::Float3, "aPosition"},
			{ ShaderDataType::Float4, "aColor"		},
			{ ShaderDataType::Int,    "aEntityID"}
			});
		s_2Ddata.LineVertexArray->AddVertexBuffer(s_2Ddata.LineVertexBuffer);
		s_2Ddata.LineVertexBufferBase = new LineVertex[s_2Ddata.MaxVertices];

		s_2Ddata.WhiteTexture = Texture2D::Create(TextureSpecification());
		uint32_t whiteTextureData = 0xffffffff;
		s_2Ddata.WhiteTexture->SetData(Buffer(&whiteTextureData, sizeof(uint32_t)));

		int32_t samplers[s_2Ddata.MaxTextureSlots];
		for (uint32_t i = 0; i < s_2Ddata.MaxTextureSlots; i++)
			samplers[i] = i;

		// Set first texture slot to 0
		s_2Ddata.TextureSlots[0] = s_2Ddata.WhiteTexture;
		//s_2Ddata.FontAtlasTextureSlots[0] = Font::GetDefault()->GetAtlasTexture();

		s_2Ddata.QuadVertexPositions[0] = { -0.5f, -0.5f, 0.0f, 1.0f };
		s_2Ddata.QuadVertexPositions[1] = {	 0.5f, -0.5f, 0.0f, 1.0f };
		s_2Ddata.QuadVertexPositions[2] = {	 0.5f,  0.5f, 0.0f, 1.0f };
		s_2Ddata.QuadVertexPositions[3] = { -0.5f,  0.5f, 0.0f, 1.0f };

		s_2Ddata.QuadShader = Renderer::GetGShader("Quad2D");
		s_2Ddata.TextShader = Renderer::GetGShader("Text");
		s_2Ddata.CircleShader = Renderer::GetGShader("Circle2D");
		s_2Ddata.LineShader = Renderer::GetGShader("Line2D");
	}

	void Renderer2D::Shutdown()
	{
		delete[] s_2Ddata.QuadVertexBufferBase;
	}

	void Renderer2D::BeginScene()
	{
		StartBatch();
	}

	void Renderer2D::EndScene()
	{
		Flush();
	}

	void Renderer2D::StartBatch()
	{
		s_2Ddata.QuadIndexCount = 0;
		s_2Ddata.QuadVertexBufferPtr = s_2Ddata.QuadVertexBufferBase;

		s_2Ddata.TextIndexCount = 0;
		s_2Ddata.TextVertexBufferPtr = s_2Ddata.TextVertexBufferBase;

		s_2Ddata.CircleIndexCount = 0;
		s_2Ddata.CircleVertexBufferPtr = s_2Ddata.CircleVertexBufferBase;

		s_2Ddata.LineVertexCount = 0;
		s_2Ddata.LineVertexBufferPtr = s_2Ddata.LineVertexBufferBase;

		s_2Ddata.TextureSlotIndex = 1;
		s_2Ddata.FontAtlasTextureIndex = 0;
	}

	void Renderer2D::Flush()
	{
		if (s_2Ddata.TextIndexCount)
		{
			uint32_t dataSize = (uint32_t)((uint8_t*)s_2Ddata.TextVertexBufferPtr - (uint8_t*)s_2Ddata.TextVertexBufferBase);
			s_2Ddata.TextVertexBuffer->SetData(s_2Ddata.TextVertexBufferBase, dataSize);

			auto buf = s_2Ddata.TextVertexBufferBase;
			// Bind textures
			for (uint32_t i = 0; i < s_2Ddata.FontAtlasTextureIndex; i++)
				s_2Ddata.FontAtlasTextureSlots[i]->Bind(i);

			s_2Ddata.TextShader->Enable();

			RenderCommand::DrawIndexed(s_2Ddata.TextVertexArray, s_2Ddata.TextIndexCount);
			s_2Ddata.Stats.DrawCalls++;
		}

		if (s_2Ddata.LineVertexCount)
		{
			uint32_t dataSize = (uint32_t)((uint8_t*)s_2Ddata.LineVertexBufferPtr - (uint8_t*)s_2Ddata.LineVertexBufferBase);
			s_2Ddata.LineVertexBuffer->SetData(s_2Ddata.LineVertexBufferBase, dataSize);

			s_2Ddata.LineShader->Enable();

			RenderCommand::DrawLines(s_2Ddata.LineVertexArray, s_2Ddata.LineVertexCount);
			s_2Ddata.Stats.DrawCalls++;
		}

		if (s_2Ddata.CircleIndexCount)
		{
			uint32_t dataSize = (uint32_t)((uint8_t*)s_2Ddata.CircleVertexBufferPtr - (uint8_t*)s_2Ddata.CircleVertexBufferBase);
			s_2Ddata.CircleVertexBuffer->SetData(s_2Ddata.CircleVertexBufferBase, dataSize);

			s_2Ddata.CircleShader->Enable();

			RenderCommand::DrawIndexed(s_2Ddata.CircleVertexArray, s_2Ddata.CircleIndexCount);
			s_2Ddata.Stats.DrawCalls++;
		}

		if (s_2Ddata.QuadIndexCount)
		{
			uint32_t dataSize = (uint32_t)((uint8_t*)s_2Ddata.QuadVertexBufferPtr - (uint8_t*)s_2Ddata.QuadVertexBufferBase);
			s_2Ddata.QuadVertexBuffer->SetData(s_2Ddata.QuadVertexBufferBase, dataSize);

			// Bind textures
			for (uint32_t i = 0; i < s_2Ddata.TextureSlotIndex; i++)
				s_2Ddata.TextureSlots[i]->Bind(i);

			s_2Ddata.QuadShader->Enable();

			RenderCommand::DrawIndexed(s_2Ddata.QuadVertexArray, s_2Ddata.QuadIndexCount);
			s_2Ddata.Stats.DrawCalls++;
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

	void Renderer2D::DrawQuad(const glm::mat4& transform, const glm::vec4& color, int entityID)
	{
		constexpr size_t quadVertexCount = 4;
		const float textureIndex = 0.0f; // White Texture
		constexpr glm::vec2 textureCoords[] = { { 0.0f, 0.0f }, { 1.0f, 0.0f }, { 1.0f, 1.0f }, { 0.0f, 1.0f } };
		const glm::vec2& tilingFactor = glm::vec2(1.0f);

		if (s_2Ddata.QuadIndexCount >= Renderer2DData::MaxIndices)
			NextBatch();

		for (size_t i = 0; i < quadVertexCount; i++)
		{
			s_2Ddata.QuadVertexBufferPtr->Position = transform * s_2Ddata.QuadVertexPositions[i];
			s_2Ddata.QuadVertexBufferPtr->Color = color;
			s_2Ddata.QuadVertexBufferPtr->TexCoord = textureCoords[i];
			s_2Ddata.QuadVertexBufferPtr->TexIndex = textureIndex;
			s_2Ddata.QuadVertexBufferPtr->TilingFactor = tilingFactor;
			s_2Ddata.QuadVertexBufferPtr->EntityID = entityID;
			s_2Ddata.QuadVertexBufferPtr++;
		}

		s_2Ddata.QuadIndexCount += 6;
		s_2Ddata.Stats.QuadCount++;
	}

	void Renderer2D::DrawQuad(const glm::mat4& transform, const std::shared_ptr<Texture2D>& texture, const glm::vec2& tilingFactor, const glm::vec4& tintColor, int entityID)
	{
		OGN_CORE_VERIFY(texture);

		constexpr size_t quadVertexCount = 4;
		constexpr glm::vec2 textureCoords[] = { { 0.0f, 0.0f }, { 1.0f, 0.0f }, { 1.0f, 1.0f }, { 0.0f, 1.0f } };

		if (s_2Ddata.QuadIndexCount >= Renderer2DData::MaxIndices)
			NextBatch();

		float textureIndex = 0.0f;
		for (uint32_t i = 1; i < s_2Ddata.TextureSlotIndex; i++)
		{
			if (*s_2Ddata.TextureSlots[i] == *texture)
			{
				textureIndex = (float)i;
				break;
			}
		}

		if (textureIndex == 0.0f)
		{
			if (s_2Ddata.TextureSlotIndex >= Renderer2DData::MaxTextureSlots)
				NextBatch();

			textureIndex = (float)s_2Ddata.TextureSlotIndex;
			s_2Ddata.TextureSlots[s_2Ddata.TextureSlotIndex] = texture;
			s_2Ddata.TextureSlotIndex++;
		}

		for (size_t i = 0; i < quadVertexCount; i++)
		{
			s_2Ddata.QuadVertexBufferPtr->Position = transform * s_2Ddata.QuadVertexPositions[i];
			s_2Ddata.QuadVertexBufferPtr->Color = tintColor;
			s_2Ddata.QuadVertexBufferPtr->TexCoord = textureCoords[i];
			s_2Ddata.QuadVertexBufferPtr->TexIndex = textureIndex;
			s_2Ddata.QuadVertexBufferPtr->TilingFactor = tilingFactor;
			s_2Ddata.QuadVertexBufferPtr->EntityID = entityID;
			s_2Ddata.QuadVertexBufferPtr++;
		}
		s_2Ddata.QuadIndexCount += 6;
		s_2Ddata.Stats.QuadCount++;
	}

	void Renderer2D::DrawQuad(const glm::mat4& transform, const std::shared_ptr<SubTexture2D>& subTexture, const glm::vec2& tilingFactor, const glm::vec4& tintColor, int entityID)
	{
		constexpr int QuadVertexCount = 4;
		const glm::vec2* textureCoords = subTexture->GetTexCoords();
		const std::shared_ptr<Texture2D> texture = subTexture->GetTexture();

		if (s_2Ddata.QuadIndexCount >= Renderer2DData::MaxIndices)
			NextBatch();

		float textureIndex = 0.0f;
		for (uint32_t i = 1; i < s_2Ddata.TextureSlotIndex; i++)
		{
			if (*s_2Ddata.TextureSlots[i] == *texture)
			{
				textureIndex = (float)i;
				break;
			}
		}

		if (textureIndex == 0.0f)
		{
			if (s_2Ddata.TextureSlotIndex >= Renderer2DData::MaxTextureSlots)
				NextBatch();

			textureIndex = (float)s_2Ddata.TextureSlotIndex;
			s_2Ddata.TextureSlots[s_2Ddata.TextureSlotIndex] = texture;
			s_2Ddata.TextureSlotIndex++;
		}

		for (size_t i = 0; i < QuadVertexCount; i++)
		{
			s_2Ddata.QuadVertexBufferPtr->Position = transform * s_2Ddata.QuadVertexPositions[i];
			s_2Ddata.QuadVertexBufferPtr->Color = tintColor;
			s_2Ddata.QuadVertexBufferPtr->TexCoord = textureCoords[i];
			s_2Ddata.QuadVertexBufferPtr->TexIndex = textureIndex;
			s_2Ddata.QuadVertexBufferPtr->TilingFactor = tilingFactor;
			s_2Ddata.QuadVertexBufferPtr++;
		}
		s_2Ddata.QuadIndexCount += 6;
		s_2Ddata.Stats.QuadCount++;
	}

	void Renderer2D::DrawRotatedQuad(const glm::vec2& position, const glm::vec2& size, float rotation, const glm::vec4& color)
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

	void Renderer2D::DrawRect(const glm::vec3& position, const glm::vec2& size, glm::vec4& color, int entityID)
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

	void Renderer2D::DrawRect(const glm::mat4& transform, glm::vec4& color, int entityID)
	{
		glm::vec3 lineVertices[4];

		for (size_t i = 0; i < 4; i++)
			lineVertices[i] = transform * s_2Ddata.QuadVertexPositions[i];

		DrawLine(lineVertices[0], lineVertices[1], color);
		DrawLine(lineVertices[1], lineVertices[2], color);
		DrawLine(lineVertices[2], lineVertices[3], color);
		DrawLine(lineVertices[3], lineVertices[0], color);
	}

	void Renderer2D::DrawCircle(const glm::mat4& transform, const glm::vec4& color, float thickness, float fade, int entityID)
	{
		for (size_t i = 0; i < 4; i++)
		{
			s_2Ddata.CircleVertexBufferPtr->WorldPosition = transform * s_2Ddata.QuadVertexPositions[i];
			s_2Ddata.CircleVertexBufferPtr->LocalPosition = s_2Ddata.QuadVertexPositions[i] * 2.0f;
			s_2Ddata.CircleVertexBufferPtr->Color = color;
			s_2Ddata.CircleVertexBufferPtr->Thickness = thickness;
			s_2Ddata.CircleVertexBufferPtr->Fade = fade;
			s_2Ddata.CircleVertexBufferPtr->EntityID = entityID;
			s_2Ddata.CircleVertexBufferPtr++;
		}

		s_2Ddata.CircleIndexCount += 6;
		s_2Ddata.Stats.CircleCount++;
	}

	void Renderer2D::DrawLine(const glm::vec3& p0, const glm::vec3& p1, glm::vec4& color, int entityID)
	{
		if (s_2Ddata.LineVertexCount >= Renderer2DData::MaxLines)
			NextBatch();

		s_2Ddata.LineVertexBufferPtr->Position = p0;
		s_2Ddata.LineVertexBufferPtr->Color = color;
		s_2Ddata.LineVertexBufferPtr->EntityID = entityID;
		s_2Ddata.LineVertexBufferPtr++;

		s_2Ddata.LineVertexBufferPtr->Position = p1;
		s_2Ddata.LineVertexBufferPtr->Color = color;
		s_2Ddata.LineVertexBufferPtr->EntityID = entityID;
		s_2Ddata.LineVertexBufferPtr++;

		s_2Ddata.LineVertexCount += 2;
		s_2Ddata.Stats.LineCount++;
	}

	void Renderer2D::DrawSprite(const glm::mat4& transform, SpriteRenderer2DComponent& src, int entityID)
	{
		std::shared_ptr<Texture2D> texture = AssetManager::GetAsset<Texture2D>(src.Texture);
		if (texture)
			DrawQuad(transform, texture, src.TillingFactor, src.Color, entityID);
		else
			DrawQuad(transform, src.Color, entityID);
	}

	void Renderer2D::DrawString(const std::string& string, std::shared_ptr<Font> font, const glm::mat4& transform, const TextParams& textParams, int entityID)
	{
		if (s_2Ddata.FontAtlasTextureIndex >= Renderer2DData::MaxTextureSlots)
			NextBatch();
		
		const auto& fontGeometry = font->GetMSDFData()->FontGeometry;
		const auto& metrics = fontGeometry.getMetrics();

		std::shared_ptr<Texture2D> fontAtlas = font->GetAtlasTexture();

		float textureIndex = 0;
		for (uint32_t i = 0; i < s_2Ddata.FontAtlasTextureIndex; i++)
		{
			if (*s_2Ddata.FontAtlasTextureSlots[i] == *fontAtlas)
			{
				s_2Ddata.FontAtlasTextureSlots[i] = fontAtlas;
				textureIndex = (float)i;
				break;
			}
		}

		if(textureIndex == 0.0f)
		{
			if (s_2Ddata.FontAtlasTextureIndex >= Renderer2DData::MaxTextureSlots)
				NextBatch();

			textureIndex = (float)s_2Ddata.FontAtlasTextureIndex;
			s_2Ddata.FontAtlasTextureSlots[s_2Ddata.FontAtlasTextureIndex] = fontAtlas;
			s_2Ddata.FontAtlasTextureIndex++;
		}
		
		double x = 0.0;
		double fsScale = 1.0 / (metrics.ascenderY - metrics.descenderY);

		double y = 0.0;

		const float spaceGlyphAdvance = fontGeometry.getGlyph(' ')->getAdvance();

		for (size_t i = 0; i < string.size(); i++)
		{
			char character = string[i];
			if (character == '\r')
				continue;

			if (character == '\n')
			{
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
				glyph = fontGeometry.getGlyph('?');
			if (!glyph)
				return;

			double al, ab, ar, at;
			glyph->getQuadAtlasBounds(al, ab, ar, at);
			glm::vec2 texCoordMin(float(al), (float)ab);
			glm::vec2 texCoordMax(float(ar), (float) at);

			double pl, pb, pr, pt;
			glyph->getQuadPlaneBounds(pl, pb, pr, pt);
			glm::vec2 quadMin((float)pl, (float)pb);
			glm::vec2 quadMax((float)pr, (float)pt);

			quadMin *= fsScale, quadMax *= fsScale;
			quadMin += glm::vec2(x, y);
			quadMax += glm::vec2(x, y);

			float texelWidth = 1.0f / s_2Ddata.FontAtlasTextureSlots[(int)textureIndex]->GetWidth();
			float texelHeight = 1.0f / s_2Ddata.FontAtlasTextureSlots[(int)textureIndex]->GetHeight();

			texCoordMin *= glm::vec2(texelWidth, texelHeight);
			texCoordMax *= glm::vec2(texelWidth, texelHeight);

			// Render Here
			s_2Ddata.TextVertexBufferPtr->Position = transform * glm::vec4(quadMin, 0.0f, 1.0f);
			s_2Ddata.TextVertexBufferPtr->Color = textParams.Color;
			s_2Ddata.TextVertexBufferPtr->TexCoord = texCoordMin;
			s_2Ddata.TextVertexBufferPtr->TexIndex = textureIndex;
			s_2Ddata.TextVertexBufferPtr->EntityID = entityID;
			s_2Ddata.TextVertexBufferPtr++;

			s_2Ddata.TextVertexBufferPtr->Position = transform * glm::vec4(quadMin.x, quadMax.y, 0.0f, 1.0f);
			s_2Ddata.TextVertexBufferPtr->Color = textParams.Color;
			s_2Ddata.TextVertexBufferPtr->TexCoord = { texCoordMin.x, texCoordMax.y };
			s_2Ddata.TextVertexBufferPtr->TexIndex = textureIndex;
			s_2Ddata.TextVertexBufferPtr->EntityID = entityID;
			s_2Ddata.TextVertexBufferPtr++;

			s_2Ddata.TextVertexBufferPtr->Position = transform * glm::vec4(quadMax, 0.0f, 1.0f);
			s_2Ddata.TextVertexBufferPtr->Color = textParams.Color;
			s_2Ddata.TextVertexBufferPtr->TexCoord = texCoordMax;
			s_2Ddata.TextVertexBufferPtr->TexIndex = textureIndex;
			s_2Ddata.TextVertexBufferPtr->EntityID = entityID;
			s_2Ddata.TextVertexBufferPtr++;

			s_2Ddata.TextVertexBufferPtr->Position = transform * glm::vec4(quadMax.x, quadMin.y, 0.0f, 1.0f);
			s_2Ddata.TextVertexBufferPtr->Color = textParams.Color;
			s_2Ddata.TextVertexBufferPtr->TexCoord = { texCoordMax.x, texCoordMin.y };
			s_2Ddata.TextVertexBufferPtr->TexIndex = textureIndex;
			s_2Ddata.TextVertexBufferPtr->EntityID = entityID;
			s_2Ddata.TextVertexBufferPtr++;

			s_2Ddata.TextIndexCount += 6;
			s_2Ddata.Stats.QuadCount++;

			if (i < string.size() - 1)
			{
				double advance = glyph->getAdvance();
				char nextCharacter = string[i + 1];
				fontGeometry.getAdvance(advance, character, nextCharacter);

				float kerningOffset = 0.0f;
				x += fsScale * advance + textParams.Kerning;
			}
		}
	}

	void Renderer2D::DrawString(const std::string& string, const glm::mat4& transform, const TextComponent& component, int entityID)
	{
		DrawString(string, component.FontAsset, transform, { component.Color, component.Kerning, component.LineSpacing }, entityID);
	}

	void Renderer2D::ResetStats()
	{
		memset(&s_2Ddata.Stats, 0, sizeof(Renderer2D::Statistics));
	}

	Renderer2D::Statistics Renderer2D::GetStats()
	{
		return s_2Ddata.Stats;
	}

}