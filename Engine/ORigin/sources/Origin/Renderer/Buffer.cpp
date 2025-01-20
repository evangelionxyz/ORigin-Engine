// Copyright (c) Evangelion Manuhutu | ORigin Engine

#include "pch.h"
#include "Buffer.h"
#include "Origin/Renderer/Renderer.h"
#include "Platform/OpenGL/OpenGLBuffer.h"

namespace origin
{

	std::shared_ptr<VertexBuffer> VertexBuffer::Create(u32 size)
	{
		switch (Renderer::GetAPI())
		{
			case RendererAPI::API::None:
                OGN_CORE_ASSERT(false, "[VertexBuffer] Please specify renderer api");
				return nullptr;
			case RendererAPI::API::OpenGL: return CreateRef<OpenGLVertexBuffer>(size);
		}

		return nullptr;
	}

	std::shared_ptr<VertexBuffer> VertexBuffer::Create(void* vertices, u32 size)
	{
		switch (Renderer::GetAPI())
		{
		case RendererAPI::API::None:
            OGN_CORE_ASSERT(false, "[VertexBuffer] Please specify renderer api");
			return nullptr;
		case RendererAPI::API::OpenGL: return CreateRef<OpenGLVertexBuffer>(vertices, size);
		}

		return nullptr;
	}

	std::shared_ptr<IndexBuffer> IndexBuffer::Create(u32 size)
	{
		switch (Renderer::GetAPI())
		{
		case RendererAPI::API::None:
            OGN_CORE_ASSERT(false, "[IndexBuffer] Please specify renderer api");
			return nullptr;
		case RendererAPI::API::OpenGL: return CreateRef<OpenGLIndexBuffer>(size);
		}

		return nullptr;
	}

	std::shared_ptr<IndexBuffer> IndexBuffer::Create(void* indices, u32 count)
	{
		switch (Renderer::GetAPI())
		{
		case RendererAPI::API::None:
            OGN_CORE_ASSERT(false, "[IndexBuffer] Please specify renderer api");
			return nullptr;
		case RendererAPI::API::OpenGL: return CreateRef<OpenGLIndexBuffer>(indices, count);
		}

		return nullptr;
	}
}