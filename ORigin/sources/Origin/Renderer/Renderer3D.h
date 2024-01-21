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

		static void DrawCube(const glm::mat4& transform, const std::shared_ptr<Texture2D>& texture, const glm::vec4& tintColor, int entityID = -1);
		static void DrawCube(const glm::mat4& transform, const glm::vec4& color, int entityID = -1);
		static void DrawCube(const glm::mat4& transform, SpriteRendererComponent& sprite, int entityID = -1);
		static void DrawCube(const glm::vec3& position, glm::vec3& rotation, glm::vec3& size, SpriteRendererComponent& sprite, int entityID = -1);
		static void DrawQuad(const glm::mat4& transform, const glm::vec3& p0, const glm::vec3& p1, const glm::vec3& p2, const glm::vec3& p3, const glm::vec4& color, int entityID = -1);
		static void DrawCapsule(const glm::mat4& transform, const glm::vec4& color, float radius, float height, int entityID = -1);
		static void DrawSphere(const glm::mat4& transform, const glm::vec4& color, float radius, int entityID = -1, uint8_t segments = 10, uint8_t stacks = 10);

		static void DrawHemisphere(const glm::mat4& transform, float radius, uint8_t segments, uint8_t stacks, const glm::vec4& color, int entityID = -1);

		static void DrawRect(const glm::vec3& position, const glm::vec2& size, glm::vec4& color = glm::vec4(1.0f), int entityID = -1);
		static void DrawRect(const glm::mat4& transform, glm::vec4& color = glm::vec4(1.0f), int entityID = -1);

		// Stats
		struct Statistics
		{
			uint32_t DrawCalls = 0;
			uint32_t CubeCount = 0;

			uint32_t GetTotalVertexCount() const { return CubeCount * 24; }
			uint32_t GetTotalIndexCount() const { return CubeCount * 36; }
		};

		static void ResetStats();
		static Statistics GetStats();

	private:
		static void StartBatch();
		static void NextBatch();
	};
}

