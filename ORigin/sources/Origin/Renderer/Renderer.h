// Copyright (c) 2022 Evangelion Manuhutu | ORigin Engine

#pragma once
#include "RenderCommand.h"
#include "UniformBuffer.h"

#include "Origin\Scene\EditorCamera.h"
#include "Origin\Scene\SceneCamera.h"

#include "Origin\Renderer\Texture.h"

#include "Origin\Scene\Components.h"

namespace origin {

	class Renderer
	{
	public:
		struct CameraBuffer
		{
			glm::mat4 ViewProjection;
			glm::vec3 Position;
		};

		inline static RendererAPI::API GetAPI() { return RendererAPI::GetAPI(); }

		static bool Init();

		static void Shutdown();

		static void BeginScene(const SceneCamera& camera, const TransformComponent& cameraTransform);
		static void BeginScene(const EditorCamera& camera);

		static void EndScene();

		static void OnWindowResize(uint32_t width, uint32_t height);
		static void OnUpdate();

		static std::shared_ptr<Shader> GetGShader(const std::string& name);
		static const std::unordered_map<std::string, std::shared_ptr<Shader>> GetSaderLibrary();

		static CameraBuffer GetCameraBuffer();

		static std::shared_ptr<Texture2D> WhiteTexture;

	private:
		static void LoadShader();
	};
}