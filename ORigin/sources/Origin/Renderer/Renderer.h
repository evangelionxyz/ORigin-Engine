// Copyright (c) 2022 Evangelion Manuhutu | ORigin Engine

#pragma once
#include "Shader.h"
#include "Texture.h"
#include "RenderCommand.h"
#include "Origin\Scene\EditorCamera.h"
#include "Origin\Scene\Component\Camera.h"

namespace Origin
{
	class Renderer
	{
	public:
		static void Init();

		static void OnWindowResize(uint32_t width, uint32_t height);
		static void OnUpdate();
		static void BeginScene(OrthoCamera& camera);

		static void Shutdown();

		static void DrawLineMode(bool enable = false);
		static void Submit(const std::shared_ptr<Shader>& shader, const std::shared_ptr<VertexArray>& vertexArray, const glm::mat4& transform = glm::mat4(1.0f));

		inline static RendererAPI::API GetAPI() { return RendererAPI::GetAPI(); }

	private:
		struct SceneData
		{
			glm::mat4 ViewProjectionMatrix;
		};

		static SceneData* m_SceneData;
	};


}