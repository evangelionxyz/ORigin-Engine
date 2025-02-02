// Copyright (c) Evangelion Manuhutu | ORigin Engine

#include "pch.h"
#include "Framebuffer.h"

#include "Renderer.h"
#include "Platform/OpenGL/OpenGLFramebuffer.h"
#include "Platform/Vulkan/VulkanFramebuffer.hpp"

#include "Origin/Core/Assert.h"

namespace origin
{
	Ref<Framebuffer> Framebuffer::Create(const FramebufferSpecification& specification)
	{
		switch (Renderer::GetAPI())
		{
			case RendererAPI::API::OpenGL: return CreateRef<OpenGLFramebuffer>(specification);
			case RendererAPI::API::Vulkan: return CreateRef<VulkanFramebuffer>(specification);
		}

		OGN_CORE_ASSERT(false, "Unkonwn Framebuffer API");
		return nullptr;
	}
}
