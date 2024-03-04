// Copyright (c) 2022 Evangelion Manuhutu | ORigin Engine

#include "pch.h"
#include "Renderer.h"
#include "RenderCommand.h"

#include "Origin\Asset\AssetImporter.h"

#include "Origin\Scene\Skybox.h"
#include "Platform\OpenGL\OpenGL_Shader.h"

#include "Renderer2D.h"
#include "Renderer3D.h"

#include <glm\gtc\matrix_transform.hpp>

namespace origin {

	static Statistics s_Statistics;
	static ShaderLibrary s_ShaderLibrary;

	RenderData Renderer::s_RenderData;
	std::shared_ptr<Shader> Renderer::s_GlobalShader;
	std::shared_ptr<Texture2D> Renderer::WhiteTexture;
	std::shared_ptr<Texture2D> Renderer::BlackTexture;

	Statistics &Renderer::GetStatistics()
	{
		return s_Statistics;
	}

	bool Renderer::Init()
	{
		RenderCommand::Init();

		uint32_t whiteTextureData = 0xffffffff;
		WhiteTexture = Texture2D::Create(TextureSpecification());
		WhiteTexture->SetData(Buffer(&whiteTextureData, sizeof(uint32_t)));

		uint32_t blackTextureData = 0xff000000;
		BlackTexture = Texture2D::Create(TextureSpecification());
		BlackTexture->SetData(Buffer(&blackTextureData, sizeof(uint32_t)));

		Renderer::LoadShaders();
		Renderer2D::Init();
		Renderer3D::Init();
		return true;
	}

	void Renderer::Shutdown()
	{
		Renderer2D::Shutdown();
		Renderer3D::Shutdown();

		OGN_CORE_WARN("Renderer Shutdown");
	}

	void Renderer::OnWindowResize(uint32_t width, uint32_t height)
	{
		RenderCommand::SetViewport(0, 0, width, height);
	}

	void Renderer::SetCurrentShader(const std::shared_ptr<Shader> &shader)
	{
		s_GlobalShader = shader;
		s_GlobalShader->Enable();
	}

	std::shared_ptr<Shader> Renderer::GetGShader(const std::string& name)
	{
		return s_ShaderLibrary.Get(name);
	}

	const std::unordered_map<std::string, std::shared_ptr<Shader>> Renderer::GetShaderLibrary()
	{
		return s_ShaderLibrary.GetMap();
	}

	void Renderer::LoadShaders()
	{
		bool recompileShader = false;

		s_ShaderLibrary.Load("Line2D", "Resources/Shaders/SPIR-V/Line2D.glsl", true, recompileShader);
		s_ShaderLibrary.Load("Circle2D", "Resources/Shaders/SPIR-V/Circle2D.glsl", true, recompileShader);
		s_ShaderLibrary.Load("Quad2D", "Resources/Shaders/SPIR-V/Quad2D.glsl", true, recompileShader);
		s_ShaderLibrary.Load("Text", "Resources/Shaders/SPIR-V/TextRenderer.glsl", true, recompileShader);
		s_ShaderLibrary.Load("Cube", "Resources/Shaders/SPIR-V/Cube.glsl", true, recompileShader);
		s_ShaderLibrary.Load("Skybox", "Resources/Shaders/Skybox.glsl", false);
	}
}