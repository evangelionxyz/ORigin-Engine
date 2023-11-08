// Copyright (c) 2022 Evangelion Manuhutu | ORigin Engine

#pragma once
#include "Origin/Renderer/Texture.h"
#include "Origin/Renderer/SubTexture2D.h"
#include "Origin/Scene/Components.h"
#include "Origin\Renderer\Font.h"

#include "Origin/Renderer/VertexArray.h"
#include "Origin/Renderer/Shader.h"
#include <glm/gtc/type_ptr.hpp>

namespace origin
{
	class Renderer2D
	{
	public:
		static void Init();
		static void Shutdown();

		static void BeginScene();
		static void EndScene();
		static void Flush();

		// Primitives
		static void DrawQuad(const glm::vec2& position, const glm::vec2& size, const glm::vec4& color);
		static void DrawQuad(const glm::vec3& position, const glm::vec2& size, const glm::vec4& color);
		static void DrawQuad(const glm::vec2& position, const glm::vec2& size, const std::shared_ptr<Texture2D>& texture, const glm::vec2&tilingFactor = glm::vec2(1.0f), const glm::vec4& tintColor = glm::vec4(1.0f));
		static void DrawQuad(const glm::vec3& position, const glm::vec2& size, const std::shared_ptr<Texture2D>& texture, const glm::vec2&tilingFactor = glm::vec2(1.0f), const glm::vec4& tintColor = glm::vec4(1.0f));

		static void DrawQuad(const glm::mat4& transform, const glm::vec4& color, int entityID = -1);
		static void DrawQuad(const glm::mat4& transform, const std::shared_ptr<SubTexture2D>& subTexture, const glm::vec2&tilingFactor = glm::vec2(1.0f), const glm::vec4& tintColor = glm::vec4(1.0f), int entityID = -1);
		static void DrawQuad(const glm::mat4& transform, const std::shared_ptr<Texture2D>& texture, const glm::vec2&tilingFactor = glm::vec2(1.0f), const glm::vec4& tintColor = glm::vec4(1.0f), int entityID = -1);

		static void DrawRotatedQuad(const glm::vec2& position, const glm::vec2& size, float rotation, const glm::vec4& color);
		static void DrawRotatedQuad(const glm::vec3& position, const glm::vec2& size, float rotation, const glm::vec4& color);
		static void DrawRotatedQuad(const glm::vec2& position, const glm::vec2& size, float rotation, const std::shared_ptr<Texture2D>& texture, const glm::vec2&tilingFactor = glm::vec2(1.0f), const glm::vec4& tintColor = glm::vec4(1.0f));
		static void DrawRotatedQuad(const glm::vec3& position, const glm::vec2& size, float rotation, const std::shared_ptr<Texture2D>& texture, const glm::vec2&tilingFactor = glm::vec2(1.0f), const glm::vec4& tintColor = glm::vec4(1.0f));

		static void DrawRect(const glm::vec3& position, const glm::vec2& size, glm::vec4& color = glm::vec4(1.0f), int entityID = -1);
		static void DrawRect(const glm::mat4& transform, glm::vec4& color = glm::vec4(1.0f), int entityID = -1);

		static void DrawCircle(const glm::mat4& transform, const glm::vec4& color, float thickness, float fade = 0.0f, int entityID = -1);
		static void DrawLine(const glm::vec3& p0, const glm::vec3& p1, glm::vec4& color = glm::vec4(1.0f), int entityID = -1);
		static void DrawSprite(const glm::mat4& transform, SpriteRenderer2DComponent& src, int entityID = -1);

		struct TextParams
		{
			glm::vec4 Color = glm::vec4(1.0);
			float Kerning = 0.0f;
			float LineSpacing = 0.0f;
		};

		static void DrawString(const std::string& string, std::shared_ptr<Font> font, const glm::mat4& transform, const TextParams& textParams, int entityID = -1);
		static void DrawString(const std::string& string, const glm::mat4& transform, const TextComponent& textComponent, int entityID = -1);

		// Stats
		struct Statistics
		{
			uint32_t DrawCalls = 0;
			uint32_t QuadCount = 0;
			uint32_t CircleCount = 0;
			uint32_t LineCount = 0;

			uint32_t GetTotalVertexCount() const { return QuadCount * 4; }
			uint32_t GetTotalIndexCount() const { return QuadCount * 6; }
		};
		static void ResetStats();
		static Statistics GetStats();

	private:
		static void StartBatch();
		static void NextBatch();
	};

	struct TextVertex
	{
		glm::vec3 Position;
		glm::vec4 Color;
		glm::vec2 TexCoord;
		float TexIndex;

		// TODO: background color for outline/bg
		// Editor Only
		int EntityID;
	};

	struct QuadVertex
	{
		glm::vec3 Position;
		glm::vec4 Color;
		glm::vec2 TexCoord;
		float TexIndex;
		glm::vec2 TilingFactor;

		// Editor only
		int EntityID;
	};

	struct CircleVertex
	{
		glm::vec3 WorldPosition;
		glm::vec3 LocalPosition;
		glm::vec4 Color;
		float Thickness;
		float Fade;

		// Editor only
		int EntityID;
	};

	struct LineVertex
	{
		glm::vec3 Position;
		glm::vec4 Color;

		// Editor only
		int EntityID;
	};

	struct Renderer2DData
	{
		static const uint32_t MaxQuads = 20000;
		static const uint32_t MaxVertices = MaxQuads * 4;
		static const uint32_t MaxIndices = MaxQuads * 6;
		static const uint32_t MaxTextureSlots = 32; // TODO: RenderCaps
		std::shared_ptr<Texture2D> WhiteTexture;

		// =============================================
		// =================== Texts ===================
		// =============================================
		std::shared_ptr<VertexArray> TextVertexArray;
		std::shared_ptr<VertexBuffer> TextVertexBuffer;
		std::shared_ptr<Shader> TextShader;
		uint32_t FontAtlasTextureIndex = 0;
		std::array<std::shared_ptr<Texture2D>, MaxTextureSlots> FontAtlasTextureSlots;
		uint32_t TextIndexCount = 0;
		TextVertex* TextVertexBufferBase = nullptr;
		TextVertex* TextVertexBufferPtr = nullptr;

		// =============================================
		// =================== Quads ===================
		// =============================================
		std::shared_ptr<VertexArray> QuadVertexArray;
		std::shared_ptr<VertexBuffer> QuadVertexBuffer;
		std::shared_ptr<Shader> QuadShader;

		uint32_t QuadIndexCount = 0;
		QuadVertex* QuadVertexBufferBase = nullptr;
		QuadVertex* QuadVertexBufferPtr = nullptr;
		glm::vec4 QuadVertexPositions[4];

		// =============================================
		// =================== Circles ===================
		// =============================================
		std::shared_ptr<VertexArray> CircleVertexArray;
		std::shared_ptr<VertexBuffer> CircleVertexBuffer;
		std::shared_ptr<Shader> CircleShader;

		uint32_t CircleIndexCount = 0;
		CircleVertex* CircleVertexBufferBase = nullptr;
		CircleVertex* CircleVertexBufferPtr = nullptr;

		// =============================================
		// =================== Lines ===================
		// =============================================
		static const uint32_t MaxLines = 10000;
		static const uint32_t MaxLineVertices = MaxLines * 2;

		std::shared_ptr<VertexArray> LineVertexArray;
		std::shared_ptr<VertexBuffer> LineVertexBuffer;
		std::shared_ptr<Shader> LineShader;

		uint32_t LineVertexCount = 0;
		LineVertex* LineVertexBufferBase = nullptr;
		LineVertex* LineVertexBufferPtr = nullptr;

		std::array<std::shared_ptr<Texture2D>, MaxTextureSlots> TextureSlots;
		uint32_t TextureSlotIndex = 1; // 0 = white texture
		Renderer2D::Statistics Stats;
	};

	static Renderer2DData s_2Ddata;

}