// Copyright (c) 2022 Evangelion Manuhutu | ORigin Engine

#include "pch.h"
#include "GraphicsContext.h"
#include "Origin\Renderer\Renderer.h"
#include "Platform\OpenGL\OpenGL_Context.h"

namespace origin
{
	std::unique_ptr<GraphicsContext> GraphicsContext::Create(void* window)
	{
		switch (Renderer::GetAPI())
		{
			case RendererAPI::API::None:    OGN_CORE_ASSERT(false, "RendererAPI::None is currently not supported!"); return nullptr;
			case RendererAPI::API::OpenGL:  return std::make_unique<OpenGLContext>(static_cast<GLFWwindow*>(window));
		}

		OGN_CORE_ASSERT(false, "Unknown RendererAPI!");
		return nullptr;
	}
}
