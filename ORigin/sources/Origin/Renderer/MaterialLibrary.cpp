// Copyright (c) Evangelion Manuhutu | ORigin Engine
#include "pch.h"
#include "MaterialLibrary.h"

namespace origin
{

	void MaterialLibrary::Add(const std::string &name, const std::shared_ptr<Material> &material)
	{
		OGN_CORE_ASSERT(!Exist(name), "Shader already exists!");
		m_MaterialMap[name] = material;
	}

	bool MaterialLibrary::Exist(const std::string &name)
	{
		return m_MaterialMap.find(name) != m_MaterialMap.end();
	}

	std::shared_ptr<Material> MaterialLibrary::Get(const std::string &name)
	{
		OGN_CORE_ASSERT(Exist(name), "Shader not found at");
		return m_MaterialMap.at(name);
	}

}