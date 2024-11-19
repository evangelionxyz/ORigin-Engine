#include "MaterialEditorPanel.h"
#include "Origin/Serializer/MaterialSerializer.h"
#include "Origin/Asset/AssetManager.h"
#include "Origin/Profiler/Profiler.h"
#include "Origin/GUI/UI.h"

#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <imgui.h>

namespace origin {

MaterialEditorPanel *s_Instance = nullptr;

MaterialEditorPanel::MaterialEditorPanel()
{
    s_Instance = this;
}

void MaterialEditorPanel::SetSelectedMaterial(AssetHandle handle)
{
    m_Material = AssetManager::GetAsset<Material>(handle);
    m_CurrentFilepath = Project::GetActiveAssetDirectory() / Project::GetActive()->GetEditorAssetManager()->GetFilepath(handle);

    if (Deserialize() && !m_Open)
        Open();
}

void MaterialEditorPanel::Render()
{
    if (m_Open && m_Material)
    {
        ImGui::Begin("Material Editor", &m_Open);

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

void MaterialEditorPanel::OnEvent(Event &e)
{
    EventDispatcher dispatcher(e);
    dispatcher.Dispatch<MouseButtonPressedEvent>(OGN_BIND_EVENT_FN(MaterialEditorPanel::OnMouseButtonPressed));
    dispatcher.Dispatch<KeyPressedEvent>(OGN_BIND_EVENT_FN(MaterialEditorPanel::OnKeyPressed));
}

bool MaterialEditorPanel::OnMouseButtonPressed(MouseButtonPressedEvent &e)
{
    return false;
}

bool MaterialEditorPanel::OnKeyPressed(KeyPressedEvent &e)
{
    return false;
}

bool MaterialEditorPanel::Deserialize()
{
    return MaterialSerializer::Deserialize(m_CurrentFilepath, m_Material);
}

MaterialEditorPanel *MaterialEditorPanel::GetInstance()
{
    return s_Instance;
}

}
