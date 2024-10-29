// Copyright (c) Evangelion Manuhutu | ORigin Engine

#include "pch.h"
#include "VertexArray.h"
#include "Origin/Renderer/Renderer.h"
#include "Platform/OpenGL/OpenGLVertexArray.h"

#include "Origin/Core/Assert.h"

namespace origin
{
	std::shared_ptr<VertexArray> VertexArray::Create()
	{
		switch (Renderer::GetAPI())
		{
			case RendererAPI::API::None:   return nullptr;
			case RendererAPI::API::OpenGL: return CreateRef<OpenGLVertexArray>();
		}
		OGN_CORE_ASSERT(false, "Unkonwn VertexArray API");
		return nullptr;
	}
}