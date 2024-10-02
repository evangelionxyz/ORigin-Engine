// Copyright (c) Evangelion Manuhutu | ORigin Engine

#include "pch.h"

#include "ShaderLibrary.h"
#include "Origin/Core/Assert.h"
#include "Origin/Profiler/Profiler.h"

namespace origin
{
	/*
	* Add an existing shaders and providing
	* the shader name into the map
	*/
	void ShaderLibrary::Add(const std::string &name, const Ref<Shader> &shader)
	{
		OGN_PROFILER_FUNCTION();

		OGN_CORE_ASSERT(!Exist(name), "Shader already exists!");
		m_ShaderMap[name] = shader;
	}

	/*
	* Add an existing shaders and auto detect the shader name
	*/
	void ShaderLibrary::Add(const Ref<Shader> &shader)
	{
		OGN_PROFILER_FUNCTION();
		std::string name = shader->GetFilepath().stem().string();
		Add(name, shader);
	}

	Ref<Shader> ShaderLibrary::Load(const std::filesystem::path &filepath)
	{
		OGN_PROFILER_FUNCTION();

		Ref<Shader> shader = Shader::Create(filepath);
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
	Ref<Shader> ShaderLibrary::Load(const std::string &name, const std::filesystem::path &filepath, bool enableSpirv, bool recompileSpirv)
	{
		OGN_PROFILER_FUNCTION();
		Ref<Shader> shader = Shader::Create(filepath, enableSpirv, recompileSpirv);
		Add(name, shader);
		return shader;
	}

	/*
	* Get all shaders from the map using string name
	*/
	Ref<Shader> ShaderLibrary::Get(const std::string &name)
	{
		OGN_PROFILER_FUNCTION();
		OGN_CORE_ASSERT(Exist(name), "Shader not found at");
		return m_ShaderMap.at(name);
	}

	/*
	* Checking if the shader already in map
	*/
	bool ShaderLibrary::Exist(const std::string &name)
	{
		OGN_PROFILER_FUNCTION();
		return m_ShaderMap.find(name) != m_ShaderMap.end();
	}
}