#pragma once
#include "Origin\Renderer\Material.h"
#include "Origin\Asset\Asset.h"

#include <filesystem>

namespace origin
{
	class MaterialEditor
	{
	public:
		void SetSelectedMaterial(AssetHandle handle);
		void OnImGuiRender();

		bool Serialize(const std::filesystem::path &filepath);
		bool Deserialize();

	private:
		bool m_IsOpened = true;
		std::filesystem::path m_CurrentFilepath;
		std::shared_ptr<Material> m_Material;
	};

}
