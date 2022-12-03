// Copyright (c) 2022 Evangelion Manuhutu | ORigin Engine

#include "pch.h"
#include "Framebuffer.h"

#include "Renderer.h"
#include "Platform/OpenGL/OpenGL_Framebuffer.h"

namespace Origin
{
	std::shared_ptr<Framebuffer> Framebuffer::Create(const FramebufferSpecification& specification)
	{
		switch (Renderer::GetAPI())
		{
			case RendererAPI::API::None:			return nullptr;
			case RendererAPI::API::OpenGL:		return std::make_shared<OpenGL_Framebuffer>(specification);
		}

		OGN_CORE_ASSERT(false, "Unkonwn Framebuffer API");
		return nullptr;
	}
}
