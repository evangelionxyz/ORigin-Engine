// Copyright (c) 2022 Evangelion Manuhutu | ORigin Engine

#pragma once
#include "Component/Component.h"
#include <unordered_map>

namespace Origin {

	class ScriptLibrary
	{
	private:
		std::unordered_map<std::string, NativeScriptComponent> m_NativeScripts;

	public:
		void Add(const std::string& name, const NativeScriptComponent& nsc);

		template<typename T>
		void Add(const std::string& name);

		NativeScriptComponent Get(const std::string& name);
		bool Exist(const std::string& name);
	};
}

