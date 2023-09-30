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

	struct GlobalUBO
	{
		glm::mat4 ViewProjection;
		glm::mat4 LightSpaceMatrix;
		glm::vec3 CameraPosition;
	};

	struct RendererData
	{
		GlobalUBO globalUboData;
		std::shared_ptr<UniformBuffer> globalUbo;
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

		s_Data.globalUbo = UniformBuffer::Create(sizeof(GlobalUBO), 0);

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
		uint32_t offsetViewProjection = 0;
		uint32_t offsetLightSpaceMatrix = sizeof(glm::mat4);
		uint32_t offsetCameraPosition = offsetLightSpaceMatrix + sizeof(glm::mat4);

		s_Data.globalUboData.ViewProjection = camera.GetProjection() * glm::inverse(transform);
		s_Data.globalUboData.LightSpaceMatrix = glm::mat4(1.0f);
		s_Data.globalUboData.CameraPosition = glm::vec3(1.0f);

		s_Data.globalUbo->Bind();
		//s_Data.globalUbo->SetData(&s_Data.globalUboData.ViewProjection, sizeof(glm::mat4), offsetViewProjection);
		//s_Data.globalUbo->SetData(&s_Data.globalUboData.LightSpaceMatrix, sizeof(glm::mat4), offsetLightSpaceMatrix);
		//s_Data.globalUbo->SetData(&s_Data.globalUboData.CameraPosition, sizeof(glm::vec3), offsetCameraPosition);
		s_Data.globalUbo->SetData(&s_Data.globalUboData, sizeof(GlobalUBO));

		s_Data.globalUbo->Unbind();

		Renderer2D::BeginScene();
		Renderer3D::BeginScene();
	}

	void Renderer::BeginScene(const EditorCamera& camera)
	{

		uint32_t offsetViewProjection = 0;
		uint32_t offsetLightSpaceMatrix = sizeof(glm::mat4);
		uint32_t offsetCameraPosition = offsetLightSpaceMatrix + sizeof(glm::mat4);

		s_Data.globalUboData.ViewProjection = camera.GetViewProjection();
		s_Data.globalUboData.LightSpaceMatrix = glm::mat4(1.0f);
		s_Data.globalUboData.CameraPosition = camera.GetPosition();

		s_Data.globalUbo->Bind();
		//s_Data.globalUbo->SetData(&s_Data.globalUboData.ViewProjection, sizeof(glm::mat4), offsetViewProjection);
		//s_Data.globalUbo->SetData(&s_Data.globalUboData.LightSpaceMatrix, sizeof(glm::mat4), offsetLightSpaceMatrix);
		//s_Data.globalUbo->SetData(&s_Data.globalUboData.CameraPosition, sizeof(glm::vec3), offsetCameraPosition);
		s_Data.globalUbo->SetData(&s_Data.globalUboData, sizeof(GlobalUBO));
		s_Data.globalUbo->Unbind();

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
		
		GShaderLibrary.Load("DirLightDepthMap", "Resources/Shaders/DirLightDepthMap.glsl", false);
		GShaderLibrary.Load("PointLightDepthMap", "Resources/Shaders/PointLightDepthMap.glsl", false);

		// Load UI Texture
		GUITextures.insert(std::make_pair("CameraIcon", Texture2D::Create("Resources/UITextures/camera.png")));
		GUITextures.insert(std::make_pair("LightingIcon", Texture2D::Create("Resources/UITextures/lighting.png")));
		GUITextures.insert(std::make_pair("AudioIcon", Texture2D::Create("Resources/UITextures/audio.png")));
	}
}