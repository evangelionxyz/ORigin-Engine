// Copyright (c) Evangelion Manuhutu | ORigin Engine

#include "pch.h"

#include "UniformBuffer.h"
#include "Origin/Renderer/Renderer.h"
#include "Platform/OpenGL/OpenGLUniformBuffer.h"

#include "Origin/Core/Assert.h"

namespace origin
{
	std::shared_ptr<UniformBuffer> UniformBuffer::Create(uint32_t size, uint32_t binding)
	{
		switch (Renderer::GetAPI())
		{
		case RendererAPI::API::None:
			OGN_CORE_ASSERT(false, "RendererAPI:: None is currently not supported!");
			return nullptr;
		case RendererAPI::API::OpenGL:
			return std::make_shared<OpenGLUniformBuffer>(size, binding);
		}
		OGN_CORE_ASSERT(false, "Unkown RendererAPI!");
		return nullptr;
	}
}