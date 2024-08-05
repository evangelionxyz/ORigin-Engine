#include "MaterialEditor.h"
#include "Origin/Serializer/MaterialSerializer.h"
#include "Origin/Asset/AssetManager.h"
#include "Origin/Profiler/Profiler.h"
#include "Origin/GUI/UI.h"
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
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

			ImGui::Text("%s", m_CurrentFilepath.stem().string().c_str());
			ImVec2 buttonSize = ImVec2(100.0f, 40.0f);
			ImGui::ColorEdit4("Color", glm::value_ptr(m_Material->Color));

			ImGui::Button("Albedo", buttonSize);
			if (ImGui::BeginDragDropTarget())
			{
				if (const ImGuiPayload *payload = ImGui::AcceptDragDropPayload("CONTENT_BROWSER_ITEM"))
				{
					AssetHandle handle = *static_cast<AssetHandle *>(payload->Data);
					if (AssetManager::GetAssetType(handle) == AssetType::Texture)
					{
						OGN_CORE_INFO("[Material Editor] AlbedoMap Applied");
						m_Material->SetAlbedoMap(handle);
					}
					else
					{
						OGN_CORE_WARN("Wrong asset type!");
					}
				}
				ImGui::EndDragDropTarget();
			}

			UI::DrawFloatControl("Emission", &m_Material->BufferData.Emission);

			ImGui::Button("Metallic", buttonSize);
			if (ImGui::BeginDragDropTarget())
			{
				if (const ImGuiPayload *payload = ImGui::AcceptDragDropPayload("CONTENT_BROWSER_ITEM"))
				{
					AssetHandle handle = *static_cast<AssetHandle *>(payload->Data);
					if (AssetManager::GetAssetType(handle) == AssetType::Texture)
					{
						OGN_CORE_INFO("[Material Editor] Metallic Applied");
						m_Material->SetMetallicMap(handle);
					}
					else
					{
						OGN_CORE_WARN("Wrong asset type!");
					}
				}
				ImGui::EndDragDropTarget();
			}
			UI::DrawFloatControl("Metallic", &m_Material->BufferData.MetallicValue);

			ImGui::Button("Roughness Map", buttonSize);
			if (ImGui::BeginDragDropTarget())
			{
				if (const ImGuiPayload *payload = ImGui::AcceptDragDropPayload("CONTENT_BROWSER_ITEM"))
				{
					AssetHandle handle = *static_cast<AssetHandle *>(payload->Data);
					if (AssetManager::GetAssetType(handle) == AssetType::Texture)
					{
						// TODO: Not implemented yet
						//m_Material->RoughnessMap = handle;
					}
					else
					{
						OGN_CORE_WARN("Wrong asset type!");
					}
				}
				ImGui::EndDragDropTarget();
			}
			UI::DrawFloatControl("Roughness", &m_Material->BufferData.RoughnessValue);

			ImGui::End();
		}
	}

	bool MaterialEditor::Deserialize()
	{
		return MaterialSerializer::Deserialize(m_CurrentFilepath, m_Material);
	}
}
