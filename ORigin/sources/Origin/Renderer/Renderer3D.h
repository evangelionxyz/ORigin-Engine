// Copyright (c) 2022 Evangelion Manuhutu | ORigin Engine

#pragma once
#include "Origin\Renderer\VertexArray.h"
#include "Origin\Renderer\Buffer.h"
#include "Origin\Renderer\Shader.h"
#include "Origin\Renderer\Texture.h"

#include "Origin\Scene\Component\Component.h"
#include "Origin\Scene\Component\Camera.h"
#include "Origin\Scene\EditorCamera.h"

#include <glm\glm.hpp>

namespace Origin
{
	class Renderer3D
	{
	public:
		static void Init();
		static void Shutdown();

		static void BeginScene(const Camera& camera, const glm::mat4& transform);
		static void BeginScene(const EditorCamera& camera);

		static void EndScene();
		static void Flush();

		static void DrawCube(const glm::mat4& transform, const glm::vec4& color, int entityID = -1);
		static void DrawCube(const glm::mat4& transform, SpriteRendererComponent& sprite, int entityID = -1);

		static void DrawRect(const glm::vec3& position, const glm::vec2& size, glm::vec4& color = glm::vec4(1.0f), int entityID = -1);
		static void DrawRect(const glm::mat4& transform, glm::vec4& color = glm::vec4(1.0f), int entityID = -1);

	private:
		static void StartBatch();
		static void NextBatch();
	};
}

