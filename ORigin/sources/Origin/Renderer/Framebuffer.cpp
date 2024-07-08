// Copyright (c) Evangelion Manuhutu | ORigin Engine


#include "Framebuffer.h"

#include "Renderer.h"
#include "Platform/OpenGL/OpenGLFramebuffer.h"
#include "Origin/Core/Assert.h"

namespace origin
{
	std::shared_ptr<Framebuffer> Framebuffer::Create(const FramebufferSpecification& specification)
	{
		switch (Renderer::GetAPI())
		{
			case RendererAPI::API::None:			return nullptr;
			case RendererAPI::API::OpenGL:		return std::make_shared<OpenGL_Framebuffer>(specification);
			case RendererAPI::API::DX11:		OGN_CORE_ASSERT(false, "");
		}

		OGN_CORE_ASSERT(false, "Unkonwn Framebuffer API");
		return nullptr;
	}
}
