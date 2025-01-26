// Copyright (c) Evangelion Manuhutu | ORigin Engine

#ifndef SHADER_LIBRARY_H
#define SHADER_LIBRARY_H

#include "Shader.h"

namespace origin
{
	class ShaderLibrary
	{
	public:
		ShaderLibrary() = default;

		void Add(const Ref<Shader> &shader);
		void Add(const std::string &name, const Ref<Shader> &shader);

        Ref<Shader> Load(const std::filesystem::path &filepath);
		Ref<Shader> Load(const std::string &name, const std::filesystem::path &filepath, bool enableSpirv, bool recompileSpirv = false);

		Ref<Shader> Get(const std::string &name);

		bool Exist(const std::string &name);

		std::unordered_map<std::string, Ref<Shader>> &GetMap() { return m_ShaderMap; }
		inline size_t GetSize() const { return m_ShaderMap.size(); }

	private:
		std::unordered_map<std::string, Ref<Shader>> m_ShaderMap;
	};
}

#endif