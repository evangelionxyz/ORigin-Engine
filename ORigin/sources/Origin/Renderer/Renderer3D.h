// Copyright (c) 2022 Evangelion Manuhutu | ORigin Engine

#pragma once
#include "Origin\Renderer\VertexArray.h"
#include "Origin\Renderer\Buffer.h"
#include "Origin\Renderer\Shader.h"
#include "Origin\Renderer\Texture.h"

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

		static void BeginScene(const Camera& camera, glm::mat4& transform);
		static void BeginScene(const EditorCamera& camera);
		static void BeginScene(const glm::mat4& ViewProjection);

		static void EndScene();
		static void Flush();

		static void DrawCube(const glm::mat4& transform, const glm::vec4& color);

	private:
		
		static void StartBatch();
		static void NextBatch();
	};
}

