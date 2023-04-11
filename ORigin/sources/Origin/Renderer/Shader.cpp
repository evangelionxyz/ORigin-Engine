// Copyright (c) 2022 Evangelion Manuhutu | ORigin Engine

#include "pch.h"
#include "Shader.h"
#include "Renderer.h"
#include "Platform\OpenGL\OpenGL_Shader.h"

namespace Origin
{
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

	void ShaderLibrary::Add(const std::string& name, const std::shared_ptr<Shader>& shader)
	{
		OGN_CORE_ASSERT(!Exist(name), "Shader already exists!");
		m_Shaders[name] = shader;
	}

	void ShaderLibrary::Add(const std::shared_ptr<Shader>& shader)
	{
		auto& name = shader->GetName();
		Add(name, shader);
	}

	std::shared_ptr<Shader> ShaderLibrary::Load(const std::string& filepath)
	{
		auto shader = Shader::Create(filepath);
		Add(shader);
		return shader;
	}
	std::shared_ptr<Shader> ShaderLibrary::Load(const std::string& name, const std::string& filepath)
	{
		auto shader = Shader::Create(name, filepath);
		Add(shader);
		shader->Bind();
		return shader;
	}

	std::shared_ptr<Shader> ShaderLibrary::Get(const std::string& name)
	{
		OGN_CORE_ASSERT(Exist(name), "Shader not found!");
		return m_Shaders[name];
	}

	bool ShaderLibrary::Exist(const std::string& name)
	{
		return m_Shaders.find(name) != m_Shaders.end();
	}
}