// Copyright (c) 2022 Evangelion Manuhutu | ORigin Engine

#include "pch.h"

#include "Buffer.h"
#include "Origin/Renderer/Renderer.h"
#include "Platform/OpenGL/OpenGLBuffer.h"

namespace origin
{

	std::shared_ptr<VertexBuffer> VertexBuffer::Create(uint32_t size)
	{
		switch (Renderer::GetAPI())
		{
			case RendererAPI::API::None:			return nullptr;
			case RendererAPI::API::OpenGL:		return std::make_shared<OpenGLVertexBuffer>(size);
		}

		return nullptr;
	}

	std::shared_ptr<VertexBuffer> VertexBuffer::Create(void* vertices, uint32_t size)
	{
		switch (Renderer::GetAPI())
		{
		case RendererAPI::API::None:			return nullptr;
		case RendererAPI::API::OpenGL:		return std::make_shared<OpenGLVertexBuffer>(vertices, size);
		}

		return nullptr;
	}

	std::shared_ptr<IndexBuffer> IndexBuffer::Create(uint32_t* indices, uint32_t count)
	{
		switch (Renderer::GetAPI())
		{
		case RendererAPI::API::None:			return nullptr;
		case RendererAPI::API::OpenGL:		return std::make_shared<OpenGLIndexBuffer>(indices, count);
		}

		return nullptr;
	}

	std::shared_ptr<IndexBuffer> IndexBuffer::Create(std::vector<uint32_t> indices)
	{
		switch (Renderer::GetAPI())
		{
		case RendererAPI::API::None:			return nullptr;
		case RendererAPI::API::OpenGL:		return std::make_shared<OpenGLIndexBuffer>(indices);
		}

		return nullptr;
	}

}