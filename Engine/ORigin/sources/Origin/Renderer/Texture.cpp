// Copyright (c) Evangelion Manuhutu | ORigin Engine

#include "pch.h"

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

#include "Texture.h"
#include "Renderer.h"

#include "Platform/Vulkan/VulkanImage.hpp"
#include "Platform/OpenGL/OpenGLTexture.h"

namespace origin {
Ref<Texture2D> Texture2D::Create(const std::filesystem::path& filepath, const TextureSpecification& specification)
{
	switch (Renderer::GetAPI())
	{
	case RendererAPI::API::None:   return nullptr;
	case RendererAPI::API::OpenGL: return CreateRef<OpenGLTexture2D>(filepath, specification);
	case RendererAPI::API::Vulkan: return CreateRef<VulkanImage>(filepath, specification);
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

}