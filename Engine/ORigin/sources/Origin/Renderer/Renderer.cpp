// Copyright (c) Evangelion Manuhutu | ORigin Engine

#include "pch.h"
#include "Renderer.h"
#include "RenderCommand.h"
#include "ShaderLibrary.h"
#include "Renderer2D.h"
#include "MaterialLibrary.h"
#include "Origin/Asset/AssetImporter.h"
#include "Platform/Vulkan/VulkanRendererAPI.hpp"

#ifdef OGN_PLATFORM_WINDOWS
#	include "Platform/DX11/DX11RendererAPI.h"
#endif

#include "Platform/OpenGL/OpenGLRendererAPI.h"

namespace origin {

static Statistics s_Statistics;
static ShaderLibrary s_ShaderLibrary;
static MaterialLibrary s_MaterialLibrary;

RenderData Renderer::render_data;
Ref<Shader> Renderer::s_GlobalShader;
Ref<Texture2D> Renderer::WhiteTexture;
Ref<Texture2D> Renderer::BlackTexture;

RendererAPI *RenderCommand::s_RendererAPI = nullptr;
LightingManager *Renderer::lighting_manager = nullptr;
MaterialManager *Renderer::material_manager = nullptr;
Ref<UniformBuffer> Renderer::camera_uniform_buffer;
Ref<UniformBuffer> Renderer::skybox_uniform_buffer;

Statistics &Renderer::GetStatistics()
{
	return s_Statistics;
}

bool Renderer::Init()
{
	OGN_PROFILER_FUNCTION();

	uint32_t whiteTextureData = 0xffffffff;
	uint32_t blackTextureData = 0xff000000;

	switch (RendererAPI::GetAPI())
	{
	case RendererAPI::API::OpenGL:
	{
		RenderCommand::s_RendererAPI = new OpenGLRendererAPI;
		WhiteTexture = Texture2D::Create(TextureSpecification());
		WhiteTexture->SetData(Buffer(&whiteTextureData, sizeof(uint32_t)));
		BlackTexture = Texture2D::Create(TextureSpecification());
		BlackTexture->SetData(Buffer(&blackTextureData, sizeof(uint32_t)));
		LoadShaders();
		Renderer2D::Init();

		break;
	}
	case RendererAPI::API::Vulkan:
	{
		RenderCommand::s_RendererAPI = new VulkanRendererAPI;
		break;
	}
#ifdef OGN_PLATFORM_WINDOWS
	case RendererAPI::API::DX11:
	{
		RenderCommand::s_RendererAPI = new DX11RendererAPI;
		break;
	}
#endif
	}
	OGN_CORE_ASSERT(RenderCommand::s_RendererAPI, "[Renderer] Renderer API is null");
	
	lighting_manager = new LightingManager();
	material_manager = new MaterialManager();
	material_manager->CreateMeshMaterialSSBO();

	camera_uniform_buffer = UniformBuffer::Create(sizeof(CameraBufferData), CAMERA_BINDING);
	skybox_uniform_buffer = UniformBuffer::Create(sizeof(glm::vec4) + sizeof(f32), SKYBOX_BINDING);
	
	RenderCommand::Init();
	OGN_CORE_TRACE("[Renderer] Initialized");
	return true;
}

void Renderer::Shutdown()
{
	OGN_PROFILER_FUNCTION();

	skybox_uniform_buffer.reset();
	camera_uniform_buffer.reset();

	delete material_manager;
	delete lighting_manager;
	
	Renderer2D::Shutdown();

	OGN_CORE_TRACE("[Renderer] Shutdown");
}

void Renderer::OnWindowResize(uint32_t width, uint32_t height)
{
	RenderCommand::SetViewport(0, 0, width, height);
}

void Renderer::SetCurrentShader(const Ref<Shader> &shader)
{
	s_GlobalShader = shader;
	s_GlobalShader->Enable();
}

Ref<Shader> Renderer::GetShader(const std::string &name)
{
	return s_ShaderLibrary.Get(name);
}

ShaderLibrary &Renderer::GetShaderLibrary()
{
	return s_ShaderLibrary;
}

Ref<Material> Renderer::GetMaterial(const std::string &name)
{
	return s_MaterialLibrary.Get(name);
}

void Renderer::LoadShaders()
{
	OGN_PROFILER_FUNCTION();

	s_ShaderLibrary.Load("Line2D", "Resources/Shaders/SPIR-V/Line2D.glsl", true);
	s_ShaderLibrary.Load("Circle2D", "Resources/Shaders/SPIR-V/Circle2D.glsl", true);
	s_ShaderLibrary.Load("Quad2D", "Resources/Shaders/SPIR-V/Quad2D.glsl", true);
	s_ShaderLibrary.Load("Text", "Resources/Shaders/SPIR-V/TextRenderer.glsl", true);

	s_ShaderLibrary.Load("TestShader", "Resources/Shaders/TestShader.glsl", false);
	s_ShaderLibrary.Load("GridShader", "Resources/Shaders/InfiniteGrid.glsl", false);
	s_ShaderLibrary.Load("SkinnedMesh", "Resources/Shaders/Skinning.glsl", false);
	s_ShaderLibrary.Load("DepthMap", "Resources/Shaders/DepthMap.glsl", false);
	s_ShaderLibrary.Load("Outline", "Resources/Shaders/Outline.glsl", false);
	s_ShaderLibrary.Load("Screen", "Resources/Shaders/Screen.glsl", false);
	s_ShaderLibrary.Load("Skybox", "Resources/Shaders/Skybox.glsl", false);
}
}