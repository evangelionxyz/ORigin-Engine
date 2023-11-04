// Copyright (c) 2022 Evangelion Manuhutu | ORigin Engine

#include "pch.h"
#include "Shader.h"
#include "Renderer.h"
#include "Platform\OpenGL\OpenGL_Shader.h"

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

	// ///////////////
	// Shader Library
	// ///////////////

	/*
	* Add an existing shaders and providing 
	* the shader name into the map
	*/
	void ShaderLibrary::Add(const std::string& name, const std::shared_ptr<Shader>& shader)
	{
		OGN_CORE_ASSERT(!Exist(name), "Shader already exists!");
		m_ShaderMap[name] = shader;
	}

	/*
	* Add an existing shaders and auto detect the shader name
	*/
	void ShaderLibrary::Add(const std::shared_ptr<Shader>& shader)
	{
		std::string name = shader->GetName();
		Add(name, shader);
	}

	std::shared_ptr<Shader> ShaderLibrary::Load(const std::string& filepath)
	{
		std::shared_ptr<Shader> shader = Shader::Create(filepath);

		Add(shader);
		return shader;
	}

	/*
	* Load shader with filepath and auto create it 
	* and providing shader name into the map
	*/
	std::shared_ptr<Shader> ShaderLibrary::Load(const std::string& name, const std::string& filepath)
	{
		std::shared_ptr<Shader> shader = Shader::Create(name, filepath);

		Add(shader);
		return shader;
	}

	/*
	* Load shader with filepath and auto create it
	* and providing shader name into the map
	* 
	* can using the SPIRV compiled/recompile
	* 
	* name, filepath, enable spirv, recompile
	*/
	std::shared_ptr<Shader> ShaderLibrary::Load(const std::string& name, const std::string& filepath, bool enableSpirv, bool recompileSpirv)
	{
		std::shared_ptr<Shader> shader = Shader::Create(filepath, enableSpirv, recompileSpirv);

		Add(name, shader);
		return shader;
	}

	/*
	* Get all shaders from the map using string name 
	*/
	std::shared_ptr<Shader> ShaderLibrary::Get(const std::string& name)
	{
		OGN_CORE_ASSERT(Exist(name), "Shader not found at");
		return m_ShaderMap.at(name);
	}

	/*
	* Checking if the shader already in map
	*/
	bool ShaderLibrary::Exist(const std::string& name)
	{
		return m_ShaderMap.find(name) != m_ShaderMap.end();
	}
}