// Copyright (c) 2022 Evangelion Manuhutu | ORigin Engine

#include "pch.h"
#include "Shader.h"
#include "Renderer.h"
#include "Platform/OpenGL/OpenGLShader.h"

namespace origin {

	std::shared_ptr<Shader> Shader::Create(const std::string& filepath, bool enableSpirv, bool recompileSpirv)
	{
		switch (Renderer::GetAPI())
		{
				case RendererAPI::API::None:			return nullptr;
				case RendererAPI::API::OpenGL:		return std::make_shared<OpenGLShader>(filepath, enableSpirv, recompileSpirv);
		}
		OGN_CORE_ASSERT(false, "Unkown RendererAPI");
		return nullptr;
	}

	std::shared_ptr<Shader> Shader::Create(const std::string& name, const std::string& filepath)
	{
		switch (Renderer::GetAPI())
		{
			case RendererAPI::API::None:			return nullptr;
			case RendererAPI::API::OpenGL:		return std::make_shared<OpenGLShader>(name, filepath);
		}
		OGN_CORE_ASSERT(false, "Unkown Shader RendererAPI");
		return nullptr;
	}

	std::shared_ptr<Shader> Shader::Create(const std::string& name, const std::string& vertexSrc, const std::string& fragmentSrc)
	{
		switch (Renderer::GetAPI())
		{
		case RendererAPI::API::None:			return nullptr;
		case RendererAPI::API::OpenGL:		return std::make_shared<OpenGLShader>(name, vertexSrc, fragmentSrc);
		}

		OGN_CORE_ASSERT(false, "Unkown Shader RendererAPI");
		return nullptr;
	}

	std::shared_ptr<Shader> Shader::Create(const std::string& name, const std::string& vertexSrc, const std::string& fragmentSrc, const std::string& geometrySrc)
	{
		switch (Renderer::GetAPI())
		{
			case RendererAPI::API::None:			return nullptr;
			case RendererAPI::API::OpenGL:		return std::make_shared<OpenGLShader>(name, vertexSrc, fragmentSrc, geometrySrc);
		}

		OGN_CORE_ASSERT(false, "Unkown Shader RendererAPI");
		return nullptr;
	}

	
}