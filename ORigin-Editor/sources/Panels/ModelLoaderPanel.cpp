// Copyright (c) Evangelion Manuhutu | ORigin Engine

#include "ModelLoaderPanel.h"
#include "Origin/Asset/AssetManager.h"
#include "Origin/Scene/Components/Components.h"
#include "Origin/Scene/EntityManager.h"
#include "Origin/GUI/UI.h"

#include <imgui.h>

namespace origin
{

    struct ModelLoaderData
    {
        ModelComponent *cmp = nullptr;
        AssetHandle handle = 0;
        Scene *scene = nullptr;
        bool isOpen = false;

        UUID entityID;
    };
    
    static ModelLoaderData s_MeshRenderData;

    void ModelLoaderPanel::Show(UUID entityID, ModelComponent *component, AssetHandle handle, Scene *scene)
    {
        s_MeshRenderData.cmp = component;
        s_MeshRenderData.handle = handle;
        s_MeshRenderData.scene = scene;
        s_MeshRenderData.isOpen = true;
        s_MeshRenderData.entityID = entityID;

        ImGui::OpenPopup("Model Importer");
    }

    void ModelLoaderPanel::OnUIRender()
    {
        if (!s_MeshRenderData.cmp)
        {
            return;
        }

        ImGui::Begin("Model Importer", &s_MeshRenderData.isOpen, ImGuiWindowFlags_AlwaysAutoResize);

        std::shared_ptr<Model> model = AssetManager::GetAsset<Model>(s_MeshRenderData.cmp->Handle);
        if (model)
        {
            ImGui::Text("UUID    : %llu", s_MeshRenderData.cmp->Handle);
            ImGui::Text("Filepath: %s", model->GetFilepath().c_str());

            ImGui::Separator();

            for (auto &mesh : model->GetMeshes())
            {
                ImGui::Text(mesh.Name.c_str());
            }

            ImGui::Separator();

            if (UI::DrawButton("Confirm"))
            {
                Entity parent = s_MeshRenderData.scene->GetEntityWithUUID(s_MeshRenderData.entityID);
                for (auto &mesh : model->GetMeshes())
                {
                    Entity entity = EntityManager::CreateEntity(mesh.Name, s_MeshRenderData.scene, EntityType::Mesh);
                    auto &sm = entity.AddComponent<StaticMeshComponent>();
                    sm.Name = mesh.Name;
                    sm.HMaterial = mesh.HMaterial;

                    EntityManager::AddChild(parent, entity, s_MeshRenderData.scene);
                }

                s_MeshRenderData.isOpen = false;
            }

            ImGui::SameLine(0.0f, 10.0f);
            if (UI::DrawButton("Close"))
            {
                s_MeshRenderData.cmp->Handle = 0;
                s_MeshRenderData.isOpen = false;
            }
        }

        if (s_MeshRenderData.isOpen == false)
        {
            s_MeshRenderData.cmp = nullptr;
            model = 0;
        }

        ImGui::End();
    }
}