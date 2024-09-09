// Copyright (c) 2022-present Evangelion Manuhutu | ORigin Engine

#ifndef MATERIAL_SERIALIZER_H
#define MATERIAL_SERIALIZER_H

#include "Origin/Renderer/Material.h"
#include <filesystem>

namespace origin
{
	class MaterialSerializer
	{
	public:
		static bool Serialize(const std::filesystem::path &filepath, const std::shared_ptr<Material> &mat);
		static bool Deserialize(const std::filesystem::path &filepath, std::shared_ptr<Material> &mat);
	};
}

#endif