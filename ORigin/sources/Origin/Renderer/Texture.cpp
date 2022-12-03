// Copyright (c) 2022 Evangelion Manuhutu | ORigin Engine

#include "pch.h"
#include "Texture.h"
#include "Renderer.h"
#include "Platform\OpenGL\OpenGL_Texture.h"

namespace Origin
{
	std::shared_ptr<Texture3D> Texture3D::Create(uint32_t width, uint32_t height)
	{
		switch (Renderer::GetAPI())
		{
			case RendererAPI::API::None:			return nullptr;
			case RendererAPI::API::OpenGL:		return std::make_shared<OpenGLTexture3D>(width, height);
		}

		return nullptr;
	}

	std::shared_ptr<Texture3D> Texture3D::Create(const std::string& filepath)
	{
		switch (Renderer::GetAPI())
		{
			case RendererAPI::API::None:			return nullptr;
			case RendererAPI::API::OpenGL:		return std::make_shared<OpenGLTexture3D>(filepath);
		}

		return nullptr;
	}

	std::shared_ptr<Texture2D> Texture2D::Create(uint32_t width, uint32_t height)
	{
		switch (Renderer::GetAPI())
		{
			case RendererAPI::API::None:			return nullptr;
			case RendererAPI::API::OpenGL:		return std::make_shared<OpenGLTexture2D>(width, height);
		}

		return nullptr;
	}

	std::shared_ptr<Texture2D> Texture2D::Create(const std::string& filepath)
	{
		switch (Renderer::GetAPI())
		{
			case RendererAPI::API::None:			return nullptr;
			case RendererAPI::API::OpenGL:		return std::make_shared<OpenGLTexture2D>(filepath);
		}

		return nullptr;
	}
}