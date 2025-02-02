// Copyright (c) Evangelion Manuhutu | ORigin Engine

#ifndef RENDERER_2D_H
#define RENDERER_2D_H

#include "Origin/Renderer/Texture.h"
#include "Origin/Renderer/SubTexture2D.h"
#include "Origin/Scene/Components/Components.h"
#include "Origin/Renderer/Font.h"

#include "Vertex2D.hpp"

namespace origin {
struct Renderer2D
{
	static void Init();
	static void Shutdown();

	static void Begin(Shader *render_shader = nullptr);
	static void End();
	static void Flush();

	// Primitives
	static void DrawQuad(const Rect &rect, const glm::vec4 &color);
	static void DrawQuad(const glm::vec2& position, const glm::vec2& size, const glm::vec4& color);
	static void DrawQuad(const glm::vec3& position, const glm::vec2& size, const glm::vec4& color);
	static void DrawQuad(const glm::vec2& position, const glm::vec2& size, const Ref<Texture2D>& texture, const glm::vec2& tiling_factor = glm::vec2(1.0f), const glm::vec4& tintColor = glm::vec4(1.0f));
	static void DrawQuad(const glm::vec3& position, const glm::vec2& size, const Ref<Texture2D>& texture, const glm::vec2& tiling_factor = glm::vec2(1.0f), const glm::vec4& tintColor = glm::vec4(1.0f));

	static void DrawQuad(const glm::mat4& transform, const glm::vec4& color);
	static void DrawQuad(const glm::mat4& transform, const Ref<SubTexture2D>& subTexture, const glm::vec2& tiling_factor = glm::vec2(1.0f), const glm::vec4& tintColor = glm::vec4(1.0f));
	static void DrawQuad(const glm::mat4& transform, const Ref<Texture2D>& texture, const glm::vec2& tiling_factor = glm::vec2(1.0f), const glm::vec4& tintColor = glm::vec4(1.0f));

	static void DrawRotatedQuad(const glm::vec2& position, const glm::vec2& size, f32 rotation, const glm::vec4& color);
	static void DrawRotatedQuad(const glm::vec3& position, const glm::vec2& size, f32 rotation, const glm::vec4& color);
	static void DrawRotatedQuad(const glm::vec2& position, const glm::vec2& size, f32 rotation, const Ref<Texture2D>& texture, const glm::vec2& tiling_factor = glm::vec2(1.0f), const glm::vec4& tintColor = glm::vec4(1.0f));
	static void DrawRotatedQuad(const glm::vec3& position, const glm::vec2& size, f32 rotation, const Ref<Texture2D>& texture, const glm::vec2& tiling_factor = glm::vec2(1.0f), const glm::vec4& tintColor = glm::vec4(1.0f));

	static void DrawRect(const glm::vec3& position, const glm::vec2& size, const glm::vec4& color = glm::vec4(1.0f));
	static void DrawRect(const glm::mat4& transform, const glm::vec4& color = glm::vec4(1.0f));

    static void DrawCircle(const glm::vec3 &position, const glm::vec2 &scale, const glm::vec4 &color, f32 thickness, f32 fade = 0.0f);
	static void DrawCircle(const glm::mat4& transform, const glm::vec4& color, f32 thickness, f32 fade = 0.0f);
	static void DrawLine(const glm::vec3& p0, const glm::vec3& p1, const glm::vec4 &color = glm::vec4(1.0f));
	static void DrawSprite(const glm::mat4& transform, SpriteRenderer2DComponent& src);


	static void DrawString(const std::string& string, Ref<Font> font, const glm::mat4& transform, const TextParams& textParams, glm::vec2 *size = nullptr);
	static void DrawString(const std::string& string, const glm::mat4& transform, TextComponent& textComponent);
	
	static void StartBatch();
	static void NextBatch();
};

}

#endif