// Copyright (c) 2022 Evangelion Manuhutu | ORigin Engine

#pragma once
#include "RenderCommand.h"
#include "Origin\Renderer\UniformBuffer.h"

#include "Origin\Scene\EditorCamera.h"
#include "Origin\Scene\Component\Camera.h"


namespace Origin
{
	struct CameraData
	{
		glm::mat4 ViewProjection;
	};

	struct LightingData
	{

	};

	struct RendererData
	{
		CameraData CameraBufferData;
		std::shared_ptr<UniformBuffer> CameraUniformBuffer;
	};

	static RendererData s_RendererData;

	class Renderer
	{
	public:
		inline static RendererAPI::API GetAPI() { return RendererAPI::GetAPI(); }

		static void Init();
		static void Shutdown();

		static void BeginScene(const Camera& camera, const glm::mat4& transform);
		static void BeginScene(const EditorCamera& camera);

		static void EndScene();

		static void OnWindowResize(uint32_t width, uint32_t height);
		static void OnUpdate();

		void DrawLineMode(bool enable);
	};
}