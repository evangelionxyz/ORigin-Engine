// Copyright (c) 2022 Evangelion Manuhutu | ORigin Engine

#include "pch.h"
#include "Renderer.h"
#include "RenderCommand.h"
#include "GraphicsContext.h"

#include "Origin\Renderer\Renderer2D.h"
#include "Origin\Renderer\Renderer3D.h"
#include "Platform\OpenGL\OpenGL_Shader.h"

#include "Origin\Scene\Skybox.h"

#include <glm\gtc\matrix_transform.hpp>

namespace Origin
{

	void Renderer::Init()
	{
		RenderCommand::Init();
		Renderer2D::Init();
		Renderer3D::Init();

		s_RendererData.CameraUniformBuffer = UniformBuffer::Create(sizeof(RendererData::CameraData), 0);
		s_RendererData.LightingUniformBuffer = UniformBuffer::Create(sizeof(RendererData::LightingData), 1);
	}

	void Renderer::Shutdown()
	{
		OGN_CORE_WARN("Renderer Shutdown");
		Renderer2D::Shutdown();
		Renderer3D::Shutdown();
	}

	void Renderer::BeginScene(const Camera& camera, const glm::mat4& transform)
	{
		s_RendererData.CameraBufferData.ViewProjection = camera.GetProjection() * glm::inverse(transform);
		s_RendererData.CameraUniformBuffer->SetData(&s_RendererData.CameraBufferData.ViewProjection, sizeof(RendererData::CameraData), 0);

		s_RendererData.LightingUniformBuffer->SetData(&s_RendererData.LightingBufferData.Position, sizeof(RendererData::LightingData), 0);
		s_RendererData.LightingUniformBuffer->SetData(&s_RendererData.LightingBufferData.Color, sizeof(RendererData::LightingData), sizeof(glm::vec4));
		s_RendererData.LightingUniformBuffer->SetData(&s_RendererData.LightingBufferData.Intensity, sizeof(RendererData::LightingData), sizeof(float));

		Renderer2D::BeginScene(camera, transform);
		Renderer3D::BeginScene(camera, transform);
	}

	void Renderer::BeginScene(const EditorCamera& camera)
	{
		s_RendererData.CameraBufferData.ViewProjection = camera.GetViewProjection();
		s_RendererData.CameraUniformBuffer->SetData(&s_RendererData.CameraBufferData.ViewProjection, sizeof(RendererData::CameraData), 0);
		
		s_RendererData.LightingUniformBuffer->SetData(&s_RendererData.LightingBufferData.Position, sizeof(RendererData::LightingData), 0);
		s_RendererData.LightingUniformBuffer->SetData(&s_RendererData.LightingBufferData.Color, sizeof(RendererData::LightingData), sizeof(glm::vec4));
		s_RendererData.LightingUniformBuffer->SetData(&s_RendererData.LightingBufferData.Intensity, sizeof(RendererData::LightingData), sizeof(float));

		Renderer2D::BeginScene(camera);
		Renderer3D::BeginScene(camera);
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

	void Renderer::DrawLineMode(bool enable)
	{
		RenderCommand::DrawLineMode(enable);
	}
}