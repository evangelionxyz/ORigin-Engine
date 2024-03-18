#include "MaterialEditor.h"
#include "Origin\Serializer\MaterialSerializer.h"
#include "Origin\Asset\AssetManager.h"
#include "Origin\Asset\AssetMetadata.h"
#include "Origin\GUI\UI.h"

#include <glm\glm.hpp>
#include <glm\gtc\type_ptr.hpp>
#include <imgui.h>

namespace origin
{
	void MaterialEditor::SetSelectedMaterial(AssetHandle handle)
	{
		m_Material = AssetManager::GetAsset<Material>(handle);
		m_CurrentFilepath = Project::GetActiveAssetDirectory() / Project::GetActive()->GetEditorAssetManager()->GetFilepath(handle);

		if (Deserialize() && !m_IsOpened)
			m_IsOpened = true;
	}

	void MaterialEditor::OnImGuiRender()
	{
		if (m_IsOpened && m_Material)
		{
			ImGui::Begin("Material Editor", &m_IsOpened);
			ImGui::Button("Albedo Map");

			std::string lable = "None";
			bool isTextureValid = false;

			if (m_Material->AlbedoMap != 0)
			{
				if (AssetManager::IsAssetHandleValid(m_Material->AlbedoMap) && AssetManager::GetAssetType(m_Material->AlbedoMap) == AssetType::Texture)
				{
					const AssetMetadata &metadata = Project::GetActive()->GetEditorAssetManager()->GetMetadata(m_Material->AlbedoMap);
					lable = metadata.Filepath.filename().string();
					isTextureValid = true;
				}
				else
				{
					lable = "Invalid";
				}
			}

			ImVec2 buttonLabelSize = ImGui::CalcTextSize(lable.c_str());
			buttonLabelSize.x += 20.0f;
			const auto buttonLabelWidth = glm::max<float>(100.0f, buttonLabelSize.x);
			ImGui::Button(lable.c_str(), ImVec2(buttonLabelWidth, 0.0f));
			ImGui::ColorEdit4("Color", glm::value_ptr(m_Material->BufferData.Color));
			UI::DrawVecControl("Emission", &m_Material->BufferData.Emission);
			UI::DrawVecControl("Metallic", &m_Material->BufferData.MetallicValue);
			UI::DrawVecControl("Roughness", &m_Material->BufferData.RoughnessValue);

			if (ImGui::Button("Save"))
			{
				OGN_CORE_INFO("[MaterialEditor] Material Saved");
				MaterialSerializer::Serialize(m_CurrentFilepath, m_Material);
			}

			ImGui::End();
		}
	}

	bool MaterialEditor::Serialize(const std::filesystem::path filepath)
	{
		m_CurrentFilepath = filepath;
		return MaterialSerializer::Serialize(filepath, m_Material);
	}

	bool MaterialEditor::Deserialize()
	{
		return MaterialSerializer::Deserialize(m_CurrentFilepath, m_Material);
	}

}
