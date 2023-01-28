// Copyright (c) 2022 Evangelion Manuhutu | ORigin Engine

#include "pch.h"
#include "VertexArray.h"
#include "Origin\Renderer\Renderer.h"
#include "Platform\OpenGL\OpenGL_VertexArray.h"

#include "Origin\Core\OriginCore.h"

namespace Origin
{
	std::shared_ptr<VertexArray> VertexArray::Create()
	{
		switch (Renderer::GetAPI())
		{
			case RendererAPI::API::None:			return nullptr;
			case RendererAPI::API::OpenGL:		return  std::make_shared<OpenGLVertexArray>();
		}
		OGN_CORE_ASSERT(false, "Unkonwn VertexArray API");
		return nullptr;
	}
}