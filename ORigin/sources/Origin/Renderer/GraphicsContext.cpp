// Copyright (c) Evangelion Manuhutu | ORigin Engine

#include "pch.h"
#include "GraphicsContext.h"
#include "Origin\Renderer\Renderer.h"

#include "Platform\OpenGL\OpenGLContext.h"
#include "Platform\DX11\DX11Context.h"

namespace origin
{
	std::shared_ptr<GraphicsContext> GraphicsContext::Create()
	{
		switch (Renderer::GetAPI())
		{
			case RendererAPI::API::None:    OGN_CORE_ASSERT(false, "RendererAPI::None is currently not supported!"); return nullptr;
			case RendererAPI::API::OpenGL:  return std::make_unique<OpenGLContext>();
			case RendererAPI::API::DX11: return std::make_unique<DX11Context>();
		}

		OGN_CORE_ASSERT(false, "Unknown RendererAPI!");
		return nullptr;
	}
}
