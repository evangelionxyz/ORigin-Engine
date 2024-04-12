// Copyright (c) Evangelion Manuhutu | ORigin Engine

#include "pch.h"
#include "Renderer.h"
#include "RenderCommand.h"
#include "ShaderLibrary.h"
#include "Renderer2D.h"
#include "Renderer3D.h"
#include "MaterialLibrary.h"
#include "Origin\Asset\AssetImporter.h"
#include "Origin\Scene\Skybox.h"
#include "Platform\OpenGL\OpenGL_Shader.h"

#include <glm\gtc\matrix_transform.hpp>

namespace origin {

	static Statistics s_Statistics;
	static ShaderLibrary s_ShaderLibrary;
	static MaterialLibrary s_MaterialLibrary;

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
		PROFILER_FUNCTION();

		RenderCommand::Init();

		uint32_t whiteTextureData = 0xffffffff;
		WhiteTexture = Texture2D::Create(TextureSpecification());
		WhiteTexture->SetData(Buffer(&whiteTextureData, sizeof(uint32_t)));

		uint32_t blackTextureData = 0xff000000;
		BlackTexture = Texture2D::Create(TextureSpecification());
		BlackTexture->SetData(Buffer(&blackTextureData, sizeof(uint32_t)));

		Renderer::LoadShaders();
		Renderer::LoadMaterials();

		Renderer2D::Init();
		Renderer3D::Init();
		return true;
	}

	void Renderer::Shutdown()
	{
		PROFILER_FUNCTION();

		Renderer2D::Shutdown();
		Renderer3D::Shutdown();

		OGN_CORE_WARN("Renderer Shutdown");
	}

	void Renderer::OnWindowResize(uint32_t width, uint32_t height)
	{
		PROFILER_FUNCTION();

		RenderCommand::SetViewport(0, 0, width, height);
	}

	void Renderer::SetCurrentShader(const std::shared_ptr<Shader> &shader)
	{
		s_GlobalShader = shader;
		s_GlobalShader->Enable();
	}

	std::shared_ptr<Shader> Renderer::GetShader(const std::string &name)
	{
		return s_ShaderLibrary.Get(name);
	}

	const std::unordered_map<std::string, std::shared_ptr<Shader>> Renderer::GetShaderLibrary()
	{
		return s_ShaderLibrary.GetMap();
	}

	std::shared_ptr<Material> Renderer::GetMaterial(const std::string &name)
	{
		return s_MaterialLibrary.Get(name);
	}

	void Renderer::LoadShaders()
	{
		PROFILER_FUNCTION();

		bool recompileShader = false;

		s_ShaderLibrary.Load("Line2D", "Resources/Shaders/SPIR-V/Line2D.glsl", true, recompileShader);
		s_ShaderLibrary.Load("Circle2D", "Resources/Shaders/SPIR-V/Circle2D.glsl", true, recompileShader);
		s_ShaderLibrary.Load("Quad2D", "Resources/Shaders/SPIR-V/Quad2D.glsl", true, recompileShader);
		s_ShaderLibrary.Load("Mesh", "Resources/Shaders/SPIR-V/Mesh.glsl", true, recompileShader);
		s_ShaderLibrary.Load("Text", "Resources/Shaders/SPIR-V/TextRenderer.glsl", true, recompileShader);
		s_ShaderLibrary.Load("Cube", "Resources/Shaders/SPIR-V/Cube.glsl", true, recompileShader);
		s_ShaderLibrary.Load("Skybox", "Resources/Shaders/Skybox.glsl", false);
	}

	void Renderer::LoadMaterials()
	{
		s_MaterialLibrary.Add("DefaultMesh", Material::Create(Renderer::GetShader("Mesh")));
	}

}