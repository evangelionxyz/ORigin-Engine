// Copyright (c) 2022 Evangelion Manuhutu | ORigin Engine

#pragma once
#include "Origin\Renderer\Texture.h"
#include "Origin\Scene\Components.h"
#include <glm\glm.hpp>
#include <glm\gtc\type_ptr.hpp>

namespace origin
{
	class Renderer3D
	{
	public:
		static void Init();
		static void Shutdown();

		static void Begin(const SceneCamera& camera, const glm::mat4& camTransform);
		static void Begin(const EditorCamera& camera);
		static void End();
		static void Flush();

		static void DrawCube(const glm::mat4& transform, const glm::vec4& color, int entityID = -1);
		static void DrawQuad(const glm::mat4& transform, const glm::vec3& p0, const glm::vec3& p1, const glm::vec3& p2, const glm::vec3& p3, const glm::vec4& color, int entityID = -1);
		static void DrawCapsule(const glm::mat4& transform, const glm::vec4& color, float radius, float height, int entityID = -1);
		static void DrawSphere(const glm::mat4& transform, const glm::vec4& color, float radius, int entityID = -1, uint8_t segments = 10, uint8_t stacks = 10);

		static void DrawHemisphere(const glm::mat4& transform, float radius, uint8_t segments, uint8_t stacks, const glm::vec4& color, int entityID = -1);

		static void DrawRect(const glm::vec3& position, const glm::vec2& size, const glm::vec4& color = glm::vec4(1.0f), int entityID = -1);
		static void DrawRect(const glm::mat4& transform, const glm::vec4& color = glm::vec4(1.0f), int entityID = -1);

	private:
		static void StartBatch();
		static void NextBatch();
	};
}

