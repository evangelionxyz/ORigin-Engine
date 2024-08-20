// Copyright (c) Evangelion Manuhutu | ORigin Engine

#ifndef RENDERER_2D_H
#define RENDERER_2D_H

#include "Origin/Renderer/Texture.h"
#include "Origin/Renderer/SubTexture2D.h"
#include "Origin/Scene/Components/Components.h"
#include "Origin/Renderer/Font.h"
#include "Origin/Scene/EditorCamera.h"

namespace origin
{
	class Renderer2D
	{
	public:
		static void Init();
		static void Shutdown();

		static void Begin(const glm::mat4 &viewProjection, glm::vec3 pos = glm::vec3(0.0f));
		static void Begin(const Camera &camera, Shader *renderShader = nullptr);
		static void End();
		static void Flush();

		// Primitives
		static void DrawQuad(const glm::vec2& position, const glm::vec2& size, const glm::vec4& color);
		static void DrawQuad(const glm::vec3& position, const glm::vec2& size, const glm::vec4& color);
		static void DrawQuad(const glm::vec2& position, const glm::vec2& size, const std::shared_ptr<Texture2D>& texture, const glm::vec2&tilingFactor = glm::vec2(1.0f), const glm::vec4& tintColor = glm::vec4(1.0f));
		static void DrawQuad(const glm::vec3& position, const glm::vec2& size, const std::shared_ptr<Texture2D>& texture, const glm::vec2&tilingFactor = glm::vec2(1.0f), const glm::vec4& tintColor = glm::vec4(1.0f));

		static void DrawQuad(const glm::mat4& transform, const glm::vec4& color);
		static void DrawQuad(const glm::mat4& transform, const std::shared_ptr<SubTexture2D>& subTexture, const glm::vec2&tilingFactor = glm::vec2(1.0f), const glm::vec4& tintColor = glm::vec4(1.0f));
		static void DrawQuad(const glm::mat4& transform, const std::shared_ptr<Texture2D>& texture, const glm::vec2&tilingFactor = glm::vec2(1.0f), const glm::vec4& tintColor = glm::vec4(1.0f));

		static void DrawRotatedQuad(const glm::vec2& position, const glm::vec2& size, float rotation, const glm::vec4& color);
		static void DrawRotatedQuad(const glm::vec3& position, const glm::vec2& size, float rotation, const glm::vec4& color);
		static void DrawRotatedQuad(const glm::vec2& position, const glm::vec2& size, float rotation, const std::shared_ptr<Texture2D>& texture, const glm::vec2&tilingFactor = glm::vec2(1.0f), const glm::vec4& tintColor = glm::vec4(1.0f));
		static void DrawRotatedQuad(const glm::vec3& position, const glm::vec2& size, float rotation, const std::shared_ptr<Texture2D>& texture, const glm::vec2&tilingFactor = glm::vec2(1.0f), const glm::vec4& tintColor = glm::vec4(1.0f));

		static void DrawRect(const glm::vec3& position, const glm::vec2& size, const glm::vec4& color = glm::vec4(1.0f));
		static void DrawRect(const glm::mat4& transform, const glm::vec4& color = glm::vec4(1.0f));

		static void DrawCircle(const glm::vec3 &position, const glm::vec2 &scale, const glm::vec4& color, float thickness, float fade = 0.0f);
		static void DrawCircle(const glm::mat4& transform, const glm::vec4& color, float thickness, float fade = 0.0f);
		static void DrawLine(const glm::vec3& p0, const glm::vec3& p1, const glm::vec4 &color = glm::vec4(1.0f));
		static void DrawSprite(const glm::mat4& transform, SpriteRenderer2DComponent& src);

		struct TextParams
		{
			glm::vec4 Color = glm::vec4(1.0);
			float Kerning = 0.0f;
			float LineSpacing = 0.0f;
		};

		static void DrawString(const std::string& string, std::shared_ptr<Font> font, const glm::mat4& transform, const TextParams& textParams, glm::vec2 *size = nullptr);
		static void DrawString(const std::string& string, const glm::mat4& transform, TextComponent& textComponent);
	private:
		static void StartBatch();
		static void NextBatch();
	};
}

#endif