// Copyright (c) 2022 Evangelion Manuhutu | ORigin Engine

#include "pch.h"
#include "Renderer.h"
#include "RenderCommand.h"

#include "Origin\Scene\Skybox.h"
#include "Platform\OpenGL\OpenGL_Shader.h"

#include "Renderer2D.h"
#include "Renderer3D.h"

#include <glm\gtc\matrix_transform.hpp>

namespace origin {

	struct CameraData
	{
		glm::mat4 ViewProjection;
	};

	struct RendererData
	{
		CameraData CameraBuffer;
		std::shared_ptr<UniformBuffer> CamUBO;
	};

	static RendererData s_Data;

	static ShaderLibrary GShaderLibrary;
	static std::unordered_map<std::string, std::shared_ptr<Texture2D>> GUITextures;

	bool Renderer::Init()
	{
		RenderCommand::Init();

		Renderer::LoadShader();

		Renderer2D::Init();
		Renderer3D::Init();

		uint32_t cameraBinding = 0;
		s_Data.CamUBO = UniformBuffer::Create(sizeof(CameraData), cameraBinding);

		return true;
	}

	void Renderer::Shutdown()
	{
		Renderer2D::Shutdown();
		Renderer3D::Shutdown();

		OGN_CORE_WARN("Renderer Shutdown");
	}

	void Renderer::BeginScene(const Camera& camera, const glm::mat4& transform)
	{
		s_Data.CamUBO->Bind();

		s_Data.CameraBuffer.ViewProjection = camera.GetProjection() * glm::inverse(transform);
		s_Data.CamUBO->SetData(glm::value_ptr(s_Data.CameraBuffer.ViewProjection), sizeof(glm::mat4));

		s_Data.CamUBO->Unbind();

		Renderer2D::BeginScene();
		Renderer3D::BeginScene();
	}

	void Renderer::BeginScene(const EditorCamera& camera)
	{
		s_Data.CamUBO->Bind();

		s_Data.CameraBuffer.ViewProjection = camera.GetViewProjection();
		s_Data.CamUBO->SetData(glm::value_ptr(s_Data.CameraBuffer.ViewProjection), sizeof(glm::mat4));

		s_Data.CamUBO->Unbind();

		Renderer2D::BeginScene();
		Renderer3D::BeginScene();
	}

	void Renderer::EndScene()
	{
		Renderer2D::EndScene();
		Renderer3D::EndScene();
	}

	void Renderer::OnWindowResize(uint32_t width, uint32_t height)
	{
		RenderCommand::SetViewport(0, 0, width, height);
	}

	void Renderer::OnUpdate()
	{
		RenderCommand::ClearColor({ 0.1f, 0.1f, 0.1f, 1.0f });
		RenderCommand::Clear();
	}

	std::shared_ptr<Shader> Renderer::GetGShader(const std::string& name)
	{
		return GShaderLibrary.Get(name);
	}

	std::shared_ptr<Texture2D> Renderer::GetGTexture(const std::string& name)
	{
		auto& it = GUITextures.find(name);
		if (it != GUITextures.end())
			return it->second;

		return nullptr;
	}

	const std::unordered_map<std::string, std::shared_ptr<Shader>> Renderer::GetSaderLibrary()
	{
		return GShaderLibrary.GetMap();
	}

	void Renderer::DrawLineMode(bool enable)
	{
		RenderCommand::DrawLineMode(enable);
	}

	void Renderer::LoadShader()
	{
		// Load All Default shaders
		bool recompileShader = false;

		GShaderLibrary.Load("Line2D", "Resources/Shaders/Line2D.glsl", true, recompileShader);
		GShaderLibrary.Load("Circle2D", "Resources/Shaders/Circle2D.glsl", true, recompileShader);
		GShaderLibrary.Load("Quad2D", "Resources/Shaders/Quad2D.glsl", true, recompileShader);
		GShaderLibrary.Load("Text", "Resources/Shaders/TextRenderer.glsl", true, recompileShader);

		GShaderLibrary.Load("Cube", "Resources/Shaders/Cube.glsl", true, recompileShader);
		GShaderLibrary.Load("Mesh", "Resources/Shaders/Mesh.glsl", false);
		GShaderLibrary.Load("Mesh2", "Resources/Shaders/Mesh2.glsl", false);
		GShaderLibrary.Load("Skybox", "Resources/Shaders/Skybox.glsl", false);

		// Load UI Texture
		GUITextures.insert(std::make_pair("CameraIcon", Texture2D::Create("Resources/UITextures/camera.png")));
		GUITextures.insert(std::make_pair("LightingIcon", Texture2D::Create("Resources/UITextures/lighting.png")));
		GUITextures.insert(std::make_pair("AudioIcon", Texture2D::Create("Resources/UITextures/audio.png")));
	}
}