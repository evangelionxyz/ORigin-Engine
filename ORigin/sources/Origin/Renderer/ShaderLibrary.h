// Copyright (c) 2022-present Evangelion Manuhutu | ORigin Engine

#ifndef SHADER_LIBRARY_H
#define SHADER_LIBRARY_H

#include "Shader.h"

namespace origin
{
	class ShaderLibrary
	{
	public:
		ShaderLibrary() = default;

		void Add(const std::shared_ptr<Shader> &shader);
		void Add(const std::string &name, const std::shared_ptr<Shader> &shader);

		std::shared_ptr<Shader> Load(const std::filesystem::path &filepath);
		std::shared_ptr<Shader> Load(const std::string &name, const std::filesystem::path &filepath, bool enableSpirv, bool recompileSpirv = false);

		std::shared_ptr<Shader> Get(const std::string &name);

		bool Exist(const std::string &name);

		std::unordered_map<std::string, std::shared_ptr<Shader>> &GetMap() { return m_ShaderMap; }
		inline size_t GetSize() const { return m_ShaderMap.size(); }

	private:
		std::unordered_map<std::string, std::shared_ptr<Shader>> m_ShaderMap;
	};
}

#endif