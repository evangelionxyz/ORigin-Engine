// Copyright (c) Evangelion Manuhutu | ORigin Engine


#include "GraphicsContext.h"
#include "Origin/Renderer/Renderer.h"
#include "Platform/OpenGL/OpenGLContext.h"

#ifdef OGN_PLATFORM_WINDOWS
	#include "Platform/DX11/DX11Context.h"
#endif

namespace origin
{
	std::shared_ptr<GraphicsContext> GraphicsContext::Create()
	{
		switch (Renderer::GetAPI())
		{
			case RendererAPI::API::None:    OGN_CORE_ASSERT(false, "RendererAPI::None is currently not supported!"); return nullptr;
			case RendererAPI::API::OpenGL:  return std::make_unique<OpenGLContext>();
#ifdef OGN_PLATFORM_WINDOWS
			case RendererAPI::API::DX11: return std::make_unique<DX11Context>();
#endif
		}

		OGN_CORE_ASSERT(false, "Unknown RendererAPI!");
		return nullptr;
	}
}
