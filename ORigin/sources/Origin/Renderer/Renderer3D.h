// Copyright (c) 2022 Evangelion Manuhutu | ORigin Engine

#pragma once
#include "Origin/Scene/EditorCamera.h"
#include "Origin/Scene/Components/Components.h"

#include <glm/glm.hpp>

namespace origin
{
	class Renderer3D
	{
	public:
		static void Init();
		static void Shutdown();

		static void Begin(const Camera &camera);
		static void End();
		static void Flush();

		static void DrawCube(const glm::mat4& transform, const glm::vec4& color);
		static void DrawQuad(const glm::mat4& transform, const glm::vec3& p0, const glm::vec3& p1, const glm::vec3& p2, const glm::vec3& p3, const glm::vec4& color);
		static void DrawCapsule(const glm::mat4& transform, const glm::vec4& color, float radius, float height);
		static void DrawSphere(const glm::mat4& transform, const glm::vec4& color, float radius, uint8_t segments = 9, uint8_t stacks = 6);

		static void DrawHemisphere(const glm::mat4& transform, float radius, uint8_t segments, uint8_t stacks, const glm::vec4& color);

		static void DrawRect(const glm::vec3& position, const glm::vec2& size, const glm::vec4& color = glm::vec4(1.0f));
		static void DrawRect(const glm::mat4& transform, const glm::vec4& color = glm::vec4(1.0f));

	private:
		static void StartBatch();
		static void NextBatch();
	};
}

