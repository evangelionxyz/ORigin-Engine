// Copyright (c) Evangelion Manuhutu | ORigin Engine

#include "pch.h"
#include "Texture.h"
#include "Renderer.h"

#include "Platform/OpenGL/OpenGLTexture.h"

namespace origin {
Ref<Texture2D> Texture2D::Create(const std::filesystem::path& filepath, const TextureSpecification& specification)
{
	switch (Renderer::GetAPI())
	{
	case RendererAPI::API::None:   return nullptr;
	case RendererAPI::API::OpenGL: return CreateRef<OpenGLTexture2D>(filepath, specification);
	}

	return nullptr;
}

Ref<Texture2D> Texture2D::Create(const TextureSpecification& specification, Buffer data)
{
	switch (Renderer::GetAPI())
	{
	case RendererAPI::API::None:   return nullptr;
	case RendererAPI::API::OpenGL: return CreateRef<OpenGLTexture2D>(specification, data);
	}

	return nullptr;
}

Ref<Texture2D> Texture2D::Create(const aiTexture *embedded_texture)
{
    switch (Renderer::GetAPI())
    {
    case RendererAPI::API::None:   return nullptr;
    case RendererAPI::API::OpenGL: return CreateRef<OpenGLTexture2D>(embedded_texture);
    }
	return nullptr;
}

Ref<TextureCube> TextureCube::Create(uint32_t width, uint32_t height)
{
	switch (Renderer::GetAPI())
	{
	case RendererAPI::API::None:   return nullptr;
	case RendererAPI::API::OpenGL: return CreateRef<OpenGLTextureCube>(width, height);
	}

	return nullptr;
}

Ref<TextureCube> TextureCube::Create(const std::string& filepath)
{
	switch (Renderer::GetAPI())
	{
	case RendererAPI::API::None:   return nullptr;
	case RendererAPI::API::OpenGL: return CreateRef<OpenGLTextureCube>(filepath);
	}

	return nullptr;
}

}