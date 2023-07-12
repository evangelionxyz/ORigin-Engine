// Copyright (c) 2022 Evangelion Manuhutu | ORigin Engine

#pragma once
#include "RenderCommand.h"
#include "UniformBuffer.h"

#include "Origin\Scene\EditorCamera.h"
#include "Origin\Scene\Camera.h"

namespace origin {

	class Renderer
	{
	public:
		inline static RendererAPI::API GetAPI() { return RendererAPI::GetAPI(); }

		static bool Init();

		static void Shutdown();

		static void BeginScene(const Camera& camera, const glm::mat4& transform);
		static void BeginScene(const EditorCamera& camera);

		static void EndScene();

		static void OnWindowResize(uint32_t width, uint32_t height);
		static void OnUpdate();

		static std::shared_ptr<Shader> GetGShader(const std::string& name);

		static const std::unordered_map<std::string, std::shared_ptr<Shader>> GetSaderLibrary();

		void DrawLineMode(bool enable);

	private:
		static void LoadShader();
	};
}