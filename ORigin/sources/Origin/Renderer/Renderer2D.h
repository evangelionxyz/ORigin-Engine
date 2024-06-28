// Copyright (c) Evangelion Manuhutu | ORigin Engine

#pragma once
#include "Origin/Renderer/Texture.h"
#include "Origin/Renderer/SubTexture2D.h"
#include "Origin/Scene/Components.h"
#include "Origin\Renderer\Font.h"
#include "Origin/Scene/SpriteSheet.h"
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

		static void Begin(const glm::mat4 &projection, const glm::vec3 &position = glm::vec3(0.0f));
		static void Begin(const SceneCamera& camera, const glm::mat4& camTransform);
		static void Begin(const EditorCamera& camera);
		static void End();
		static void Flush();

		// Primitives
		static void DrawQuad(const glm::vec2& position, const glm::vec2& size, const glm::vec4& color);
		static void DrawQuad(const glm::vec3& position, const glm::vec2& size, const glm::vec4& color);
		static void DrawQuad(const glm::vec2& position, const glm::vec2& size, const std::shared_ptr<Texture2D>& texture, const glm::vec2&tilingFactor = glm::vec2(1.0f), const glm::vec4& tintColor = glm::vec4(1.0f));
		static void DrawQuad(const glm::vec3& position, const glm::vec2& size, const std::shared_ptr<Texture2D>& texture, const glm::vec2&tilingFactor = glm::vec2(1.0f), const glm::vec4& tintColor = glm::vec4(1.0f));

		static void DrawQuad(const glm::mat4& transform, const glm::vec4& color, int entityID = -1);
		static void DrawQuad(const glm::mat4& transform, const std::shared_ptr<SubTexture2D>& subTexture, int entityID = -1, const glm::vec2&tilingFactor = glm::vec2(1.0f), const glm::vec4& tintColor = glm::vec4(1.0f));
		static void DrawQuad(const glm::mat4& transform, const std::shared_ptr<Texture2D>& texture, int entityID = -1, const glm::vec2&tilingFactor = glm::vec2(1.0f), const glm::vec4& tintColor = glm::vec4(1.0f));

		static void DrawRotatedQuad(const glm::vec2& position, const glm::vec2& size, float rotation, const glm::vec4& color);
		static void DrawRotatedQuad(const glm::vec3& position, const glm::vec2& size, float rotation, const glm::vec4& color);
		static void DrawRotatedQuad(const glm::vec2& position, const glm::vec2& size, float rotation, const std::shared_ptr<Texture2D>& texture, const glm::vec2&tilingFactor = glm::vec2(1.0f), const glm::vec4& tintColor = glm::vec4(1.0f));
		static void DrawRotatedQuad(const glm::vec3& position, const glm::vec2& size, float rotation, const std::shared_ptr<Texture2D>& texture, const glm::vec2&tilingFactor = glm::vec2(1.0f), const glm::vec4& tintColor = glm::vec4(1.0f));

		static void DrawRect(const glm::vec3& position, const glm::vec2& size, glm::vec4& color = glm::vec4(1.0f), int entityID = -1);
		static void DrawRect(const glm::mat4& transform, const glm::vec4& color = glm::vec4(1.0f), int entityID = -1);

		static void DrawCircle(const glm::vec3 &position, const glm::vec2 &scale, const glm::vec4& color, float thickness, float fade = 0.0f, int entityID = -1);
		static void DrawCircle(const glm::mat4& transform, const glm::vec4& color, float thickness, float fade = 0.0f, int entityID = -1);
		static void DrawLine(const glm::vec3& p0, const glm::vec3& p1, const glm::vec4 &color = glm::vec4(1.0f), int entityID = -1);
		static void DrawSprite(const glm::mat4& transform, SpriteRenderer2DComponent& src, int entityID = -1);

		struct TextParams
		{
			glm::vec4 Color = glm::vec4(1.0);
			float Kerning = 0.0f;
			float LineSpacing = 0.0f;
		};

		static void DrawString(const std::string& string, std::shared_ptr<Font> font, const glm::mat4& transform, const TextParams& textParams, int entityID = -1);
		static void DrawString(const std::string& string, const glm::mat4& transform, const TextComponent& textComponent, int entityID = -1);
	private:
		static void StartBatch();
		static void NextBatch();
	};
}