// Copyright (c) 2022 Evangelion Manuhutu | ORigin Engine

#include "pch.h"
#include "Skybox.h"
#include "Origin\Renderer\Renderer.h"
#include "Origin\Renderer\RenderCommand.h"

#include "Platform\OpenGL\OpenGL_Skybox.h"

#include "stb_image.h"

namespace Origin
{
	std::shared_ptr<Skybox> Skybox::Create(const std::string& filepath)
	{
		switch (RendererAPI::GetAPI())
		{
			case RendererAPI::API::None:		return nullptr;
			case RendererAPI::API::OpenGL:		return std::make_shared<OpenGLSkybox>(filepath);
		}
		return nullptr;
	}
}
