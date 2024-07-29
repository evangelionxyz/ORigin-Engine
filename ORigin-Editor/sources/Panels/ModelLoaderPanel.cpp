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
    
    static ModelLoaderData s_Data;

    void ModelLoaderPanel::Show(UUID entityID, ModelComponent *component, AssetHandle handle, Scene *scene)
    {
        s_Data.cmp = component;
        s_Data.handle = handle;
        s_Data.scene = scene;
        s_Data.isOpen = true;
        s_Data.entityID = entityID;

        ImGui::OpenPopup("Model Importer");
    }

    void ModelLoaderPanel::OnUIRender()
    {
        if (!s_Data.cmp)
        {
            return;
        }

        ImGui::Begin("Model Importer", &s_Data.isOpen, ImGuiWindowFlags_AlwaysAutoResize);

        std::shared_ptr<Model> model = AssetManager::GetAsset<Model>(s_Data.cmp->Handle);
        if (model)
        {
            ImGui::Text("UUID    : %llu", s_Data.cmp->Handle);
            ImGui::Text("Filepath: %s", model->GetFilepath().c_str());

            ImGui::Separator();

            for (auto &mesh : model->GetMeshes())
            {
                ImGui::Text(mesh.Name.c_str());
            }

            ImGui::Separator();

            if (UI::DrawButton("Confirm"))
            {
                Entity parent = s_Data.scene->GetEntityWithUUID(s_Data.entityID);
                for (auto &mesh : model->GetMeshes())
                {
                    Entity entity = EntityManager::CreateEntity(mesh.Name, s_Data.scene, EntityType::Mesh);
                    auto &sm = entity.AddComponent<StaticMeshComponent>();
                    sm.Name = mesh.Name;
                    sm.HMaterial = mesh.HMaterial;

                    EntityManager::AddChild(parent, entity, s_Data.scene);
                }

                s_Data.isOpen = false;
            }

            ImGui::SameLine(0.0f, 10.0f);
            if (UI::DrawButton("Close"))
            {
                s_Data.cmp->Handle = 0;
                s_Data.isOpen = false;
            }
        }

        if (s_Data.isOpen == false)
        {
            s_Data.cmp = nullptr;
            model = 0;
        }

        ImGui::End();
    }
}