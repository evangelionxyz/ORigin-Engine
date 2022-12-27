// Copyright (c) 2022 Evangelion Manuhutu | ORigin Engine

#include "pch.h"
#include "ScriptLibrary.h"

namespace Origin
{
	void ScriptLibrary::Add(const std::string& name, const NativeScriptComponent& nsc)
	{
		OGN_CORE_ASSERT(!Exist(name), "Script Already Added");
		m_NativeScripts[name] = nsc;
	}

	template<typename T>
	void ScriptLibrary::Add(const std::string& name)
	{
		OGN_CORE_ASSERT(!Exist(name), "Script Already Added");
		m_NativeScripts[name].Bind<T>();
	}

	NativeScriptComponent ScriptLibrary::Get(const std::string& name)
	{
		OGN_CORE_ASSERT(Exist(name), "Script not found!");
		return m_NativeScripts[name];
	}

	bool ScriptLibrary::Exist(const std::string& name)
	{
		return m_NativeScripts.find(name) != m_NativeScripts.end();
	}

}