// Copyright (c) 2022 Evangelion Manuhutu | ORigin Engine

#include "pch.h"
#include "Renderer.h"
#include "RenderCommand.h"
#include "GraphicsContext.h"

#include "Origin\Renderer\Renderer2D.h"
#include "Origin\Renderer\Renderer3D.h"
#include "Platform\OpenGL\OpenGL_Shader.h"
#include "Origin\Scene\Skybox.h"

namespace Origin
{
	void Renderer::Init()
	{
		RenderCommand::Init();
		Renderer2D::Init();
		Renderer3D::Init();
	}

	void Renderer::Shutdown()
	{
		OGN_CORE_WARN("Renderer Shutdown");
		Renderer2D::Shutdown();
		Renderer3D::Shutdown();
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