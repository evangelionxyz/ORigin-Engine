// Copyright (c) 2022-present Evangelion Manuhutu | ORigin Engine

#ifndef MATERIAL_LIBRARY_H
#define MATERIAL_LIBRARY_H

#include "Materials/Material.hpp"

namespace origin
{
	class MaterialLibrary
	{
	public:
		MaterialLibrary() = default;

		void Add(const std::string &name, const Ref<Material> &material);
		bool Exist(const std::string &name) const;

		Ref<Material> Get(const std::string &name);
		std::unordered_map<std::string, Ref<Material>> GetMap() const { return m_MaterialMap; }
		inline size_t GetSize() const { return m_MaterialMap.size(); }

	private:
		std::unordered_map<std::string, Ref<Material>> m_MaterialMap;
	};
}

#endif