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

		static void BeginScene();

		static void EndScene();
		static void Flush();

		static void DrawCube(const glm::mat4& transform, const std::shared_ptr<Texture2D>& texture, const glm::vec4& tintColor, int entityID = -1);
		static void DrawCube(const glm::mat4& transform, const glm::vec4& color, int entityID = -1);
		static void DrawCube(const glm::mat4& transform, SpriteRendererComponent& sprite, int entityID = -1);
		static void DrawCube(const glm::vec3& position, glm::vec3& rotation, glm::vec3& size, SpriteRendererComponent& sprite, int entityID = -1);

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

