// Copyright (c) Evangelion Manuhutu | ORigin Engine

#include "pch.h"
#include "GraphicsContext.h"
#include "Origin/Renderer/Renderer.h"
#include "Platform/OpenGL/OpenGLContext.h"
#include "Platform/Vulkan/VulkanContext.h"

#ifdef OGN_PLATFORM_WINDOWS
	#include "Platform/DX11/DX11Context.h"
#endif

namespace origin
{
	std::shared_ptr<GraphicsContext> GraphicsContext::s_Instance = nullptr;

	std::shared_ptr<GraphicsContext> GraphicsContext::Create()
	{
		switch (Renderer::GetAPI())
		{
			case RendererAPI::API::None:    OGN_CORE_ASSERT(false, "RendererAPI::None is currently not supported!"); return nullptr;
			case RendererAPI::API::OpenGL: 
			{
				s_Instance = std::make_shared<OpenGLContext>();
				return s_Instance;
			}
			case RendererAPI::API::Vulkan:
			{
				s_Instance = std::make_shared<VulkanContext>();
				return s_Instance;
			}
#ifdef OGN_PLATFORM_WINDOWS
			case RendererAPI::API::DX11: 
			{
				s_Instance = std::make_shared<DX11Context>();
				return std::make_unique<DX11Context>();
			}
#endif
		}

		OGN_CORE_ASSERT(false, "Unknown RendererAPI!");
		return nullptr;
	}
}
