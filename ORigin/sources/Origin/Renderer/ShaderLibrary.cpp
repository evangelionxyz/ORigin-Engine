// Copyright (c) Evangelion Manuhutu | ORigin Engine
#include "pch.h"
#include "ShaderLibrary.h"

namespace origin
{
	// ///////////////
	// Shader Library
	// ///////////////

	/*
	* Add an existing shaders and providing
	* the shader name into the map
	*/
	void ShaderLibrary::Add(const std::string &name, const std::shared_ptr<Shader> &shader)
	{
		PROFILER_FUNCTION();

		OGN_CORE_ASSERT(!Exist(name), "Shader already exists!");
		m_ShaderMap[name] = shader;
	}

	/*
	* Add an existing shaders and auto detect the shader name
	*/
	void ShaderLibrary::Add(const std::shared_ptr<Shader> &shader)
	{
		PROFILER_FUNCTION();

		std::string name = shader->GetName();
		Add(name, shader);
	}

	std::shared_ptr<Shader> ShaderLibrary::Load(const std::string &filepath)
	{
		PROFILER_FUNCTION();

		std::shared_ptr<Shader> shader = Shader::Create(filepath);

		Add(shader);
		return shader;
	}

	/*
	* Load shader with filepath and auto create it
	* and providing shader name into the map
	*/
	std::shared_ptr<Shader> ShaderLibrary::Load(const std::string &name, const std::string &filepath)
	{
		PROFILER_FUNCTION();

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
	std::shared_ptr<Shader> ShaderLibrary::Load(const std::string &name, const std::string &filepath, bool enableSpirv, bool recompileSpirv)
	{
		PROFILER_FUNCTION();

		std::shared_ptr<Shader> shader = Shader::Create(filepath, enableSpirv, recompileSpirv);

		Add(name, shader);
		return shader;
	}

	/*
	* Get all shaders from the map using string name
	*/
	std::shared_ptr<Shader> ShaderLibrary::Get(const std::string &name)
	{
		PROFILER_FUNCTION();

		OGN_CORE_ASSERT(Exist(name), "Shader not found at");
		return m_ShaderMap.at(name);
	}

	/*
	* Checking if the shader already in map
	*/
	bool ShaderLibrary::Exist(const std::string &name)
	{
		PROFILER_FUNCTION();

		return m_ShaderMap.find(name) != m_ShaderMap.end();
	}
}