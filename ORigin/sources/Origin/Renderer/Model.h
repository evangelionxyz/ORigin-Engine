#pragma once

#include "Material.h"
#include "Origin/Asset/Asset.h"

#include <memory>
#include <vector>
#include <glm/glm.hpp>

namespace origin
{
	class Scene;
	class StaticMeshComponent;
	
	class Model : public Asset
	{
	public:
		virtual const std::string &GetFilepath() = 0;
		static std::shared_ptr<Model> Create(const char *filepath, Scene *scene = nullptr);
		virtual const std::vector<StaticMeshComponent> &GetMeshes() = 0;
		
		static AssetType GetStaticType() { return AssetType::Model; }
		AssetType GetType() const override { return GetStaticType(); }
	};
}


