// Copyright (c) 2022 Evangelion Manuhutu | ORigin Engine

#include "pch.h"
#include "Renderer.h"
#include "RenderCommand.h"
#include "GraphicsContext.h"

#include "Origin\Renderer\Renderer2D.h"
#include "Origin\Renderer\Renderer3D.h"

#include "Origin\Scene\Skybox.h"
#include "Origin\Scene\Component\Lighting.h"

#include "Platform\OpenGL\OpenGL_Shader.h"
#include <glm\gtc\matrix_transform.hpp>

namespace Origin
{

	void Renderer::Init()
	{
		RenderCommand::Init();
		Renderer2D::Init();
		Renderer3D::Init();

		s_RendererData.CameraUniformBuffer = UniformBuffer::Create(sizeof(RendererData::CameraData), 0);
	}

	void Renderer::Shutdown()
	{
		OGN_CORE_WARN("Renderer Shutdown");
		Renderer2D::Shutdown();
		Renderer3D::Shutdown();
	}

	void Renderer::BeginScene(const Camera& camera, const glm::mat4& transform)
	{
		// Camera
		s_RendererData.CameraUniformBuffer->Bind();
		s_RendererData.CameraBufferData.ViewProjection = camera.GetProjection() * glm::inverse(transform);
		s_RendererData.CameraUniformBuffer->SetData(&s_RendererData.CameraBufferData, sizeof(RendererData::CameraData));
		s_RendererData.CameraUniformBuffer->Unbind();

		Renderer2D::BeginScene();
		Renderer3D::BeginScene();
	}

	void Renderer::BeginScene(const EditorCamera& camera)
	{
		s_RendererData.CameraUniformBuffer->Bind();
		s_RendererData.CameraBufferData.ViewProjection = camera.GetViewProjection();
		s_RendererData.CameraUniformBuffer->SetData(&s_RendererData.CameraBufferData, sizeof(RendererData::CameraData), 0);
		s_RendererData.CameraUniformBuffer->Unbind();

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

	void Renderer::DrawLineMode(bool enable)
	{
		RenderCommand::DrawLineMode(enable);
	}
}