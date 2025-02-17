// Copyright (c) Evangelion Manuhutu | ORigin Engine

#include "../EditorLayer.hpp"

#include "UIEditorPanel.hpp"
#include "SceneHierarchyPanel.hpp"
#include "entt/entt.hpp"

#include "Origin/GUI/UI.h"
#include "Origin/Project/Project.h"
#include "Origin/Asset/AssetManager.h"
#include "Origin/Asset/AssetMetadata.h"
#include "Origin/Scene/EntityManager.h"
#include "Origin/Asset/AssetImporter.h"
#include "Origin/Audio/AudioSource.h"
#include "Origin/Scripting/ScriptEngine.h"
#include "Origin/Renderer/Renderer.h"
#include "Origin/Renderer/Lighting/Lighting.hpp"

#include <box2d/box2d.h>
#include <misc/cpp/imgui_stdlib.h>

namespace origin {
SceneHierarchyPanel *s_Instance = nullptr;

SceneHierarchyPanel::SceneHierarchyPanel(const std::shared_ptr<Scene> &scene)
{
    s_Instance = this;

    SetActiveScene(scene);
}

SceneHierarchyPanel::~SceneHierarchyPanel()
{
}

Entity SceneHierarchyPanel::SetSelectedEntity(Entity entity)
{
    if (m_Scene->IsFocusing())
    {
        return { entt::null, nullptr };
    }

    return m_SelectedEntity = entity;
}

Entity SceneHierarchyPanel::GetSelectedEntity()
{
    if (m_SelectedEntity)
    {
        Entity entity = { m_SelectedEntity, m_Scene.get() };

        if (entity)
        {
            return entity;
        }
    }

    return { entt::null, nullptr };
}

void SceneHierarchyPanel::SetActiveScene(const std::shared_ptr<Scene> &scene, bool reset)
{
    if (reset)
    {
        m_SelectedEntity = { entt::null, nullptr };
    }

    m_Scene = scene;

    if (!m_SelectedEntity)
        return;

    UUID entityID = m_SelectedEntity.GetUUID();
    Entity newEntity = m_Scene->GetEntityWithUUID(entityID);

    if (newEntity)
    {
        if (entityID == newEntity.GetUUID())
        {
            m_SelectedEntity = newEntity;
        }
    }
    else
    {
        m_SelectedEntity = { entt::null, nullptr };
    }
}

void SceneHierarchyPanel::DestroyEntity(Entity entity)
{
    if (!m_Scene->IsFocusing())
    {
        m_SelectedEntity = { entt::null, nullptr };
        m_Scene->DestroyEntity(entity);
    }
}

void SceneHierarchyPanel::OnEvent(Event &e)
{
    EventDispatcher dispatcher(e);
    dispatcher.Dispatch<MouseButtonPressedEvent>(OGN_BIND_EVENT_FN(SceneHierarchyPanel::OnMouseButtonPressed));
    dispatcher.Dispatch<KeyPressedEvent>(OGN_BIND_EVENT_FN(SceneHierarchyPanel::OnKeyPressed));
}

SceneHierarchyPanel* SceneHierarchyPanel::GetInstance()
{
    return s_Instance;
}

bool SceneHierarchyPanel::OnMouseButtonPressed(MouseButtonPressedEvent &e)
{
    return false;
}

bool SceneHierarchyPanel::OnKeyPressed(KeyPressedEvent &e)
{
    return false;
}

void SceneHierarchyPanel::Render()
{
    EntityHierarchyPanel();
    EntityPropertiesPanel();

    if (m_BlendSpacePopUp)
    {
        ImGui::OpenPopup("Blend Space");

        if (ImGui::BeginPopupModal("Blend Space", &m_BlendSpacePopUp))
        {
            MeshComponent &comp = *(MeshComponent *)m_SendData;
            Ref<Model> model = AssetManager::GetAsset<Model>(comp.HModel);

            if (model)
            {
                for (auto &anim : model->GetAnimations())
                    ImGui::Text(anim.GetName().c_str());
            }

            ImGui::Text("Blend Space Range");
            if (UI::DrawVec2Control("Blend Min", m_BlendSpaceInfo.blend_min_range))
            {
                comp.blend_space.SetRange(m_BlendSpaceInfo.blend_min_range, m_BlendSpaceInfo.blend_max_range);
            }

            if (UI::DrawVec2Control("Blend Max", m_BlendSpaceInfo.blend_max_range))
            {
                comp.blend_space.SetRange(m_BlendSpaceInfo.blend_min_range, m_BlendSpaceInfo.blend_max_range);
            }
                

            ImGui::InputText("Name", &m_BlendSpaceInfo.name);
            UI::DrawVec2Control("Min Range", m_BlendSpaceInfo.min_range);
            UI::DrawVec2Control("Max Range", m_BlendSpaceInfo.max_range);

            if (!m_BlendSpaceInfo.name.empty())
            {
                ImGui::SameLine();
                if (ImGui::Button("Create"))
                {
                    comp.blend_space.AddAnimation(m_BlendSpaceInfo.name, m_BlendSpaceInfo.min_range, m_BlendSpaceInfo.max_range);
                }
            }

            for (auto &state : comp.blend_space.GetStates())
            {
                ImGui::PushID((i32)(u64)&state.anim_index);

                ImGui::InputText("Name", &state.name);
                UI::DrawVec2Control("Min Range", state.min_range);
                ImGui::SameLine();
                UI::DrawVec2Control("Max Range", state.max_range);

                ImGui::PopID();
            }

            ImGui::EndPopup();
        }

        if (!m_BlendSpacePopUp)
        {
            m_SendData = nullptr;
        }

    }
}

void SceneHierarchyPanel::OnUpdate(float delta_time)
{
}

Entity SceneHierarchyPanel::ShowEntityContextMenu()
{
    Entity entity{};
    if (ImGui::MenuItem("Empty"))
    {
        entity = SetSelectedEntity(EntityManager::CreateEntity("Empty", m_Scene.get(), EntityType::Entity));
    }

    if (ImGui::BeginMenu("2D"))
    {
        if (ImGui::MenuItem("Sprite"))
            entity = SetSelectedEntity(EntityManager::CreateSprite("Sprite", m_Scene.get()));
        if (ImGui::MenuItem("Circle"))
            entity = SetSelectedEntity(EntityManager::CreateCircle("Circle", m_Scene.get()));
        ImGui::EndMenu();
    }

    if (ImGui::BeginMenu("3D"))
    {
        if (ImGui::MenuItem("Empty Mesh"))
            entity = SetSelectedEntity(EntityManager::CreateMesh("Empty Mesh", m_Scene.get()));

        if (ImGui::MenuItem("Environment Map"))
            entity = SetSelectedEntity(EntityManager::CreateEnvironmentMap("Environment Map", m_Scene.get()));

        ImGui::EndMenu();
    }

    if (ImGui::BeginMenu("Lighting"))
    {
        if (ImGui::MenuItem("Directional Light"))
            entity = SetSelectedEntity(EntityManager::CreateDirectionalLighting("Directional Light", m_Scene.get()));
        if (ImGui::MenuItem("Point Light"))
            entity = SetSelectedEntity(EntityManager::CreatePointLight("Point Light", m_Scene.get()));

        ImGui::EndMenu();
    }

    if (ImGui::MenuItem("UI"))
    {
        entity = SetSelectedEntity(EntityManager::CreateUI("UI", m_Scene.get()));
    }

    if (ImGui::MenuItem("Camera"))
    {
        entity = SetSelectedEntity(EntityManager::CreateCamera("Camera", m_Scene.get()));
    }

    

    return entity;
}

void SceneHierarchyPanel::EntityHierarchyPanel()
{
    ImGui::Begin("Hierarchy");

    if (!m_Scene)
    {
        ImGui::End();
        return;
    }
    IsSceneHierarchyFocused = ImGui::IsWindowFocused();
    static ImGuiWindowFlags windowFlags = ImGuiWindowFlags_NoScrollWithMouse | ImGuiWindowFlags_NoScrollbar;
    ImGui::BeginChild("scene_hierarchy", { ImGui::GetContentRegionAvail().x, 20.0f }, 0, windowFlags);
    ImGui::Button(m_Scene->GetName().c_str(), ImGui::GetContentRegionAvail());
    if (ImGui::BeginDragDropTarget())
    {
        if (const ImGuiPayload *payload = ImGui::AcceptDragDropPayload("ENTITY_SOURCE_ITEM"))
        {
            OGN_CORE_ASSERT(payload->DataSize == sizeof(Entity), "WRONG ENTITY ITEM");
            Entity src { *static_cast<entt::entity *>(payload->Data), m_Scene.get() };
            if (src.HasParent())
            {
                Entity parent = m_Scene->GetEntityWithUUID(src.GetComponent<IDComponent>().Parent);
                parent.GetComponent<IDComponent>().RemoveChild(src.GetUUID());
                src.GetComponent<IDComponent>().Parent = 0;
            }
        }
        ImGui::EndDragDropTarget();
    }
    ImGui::EndChild();

    ImGui::BeginChild("entity_hierarchy", ImGui::GetContentRegionAvail(), 0, windowFlags);

    static ImGuiTableFlags tableFlags = ImGuiTableFlags_RowBg | ImGuiTableFlags_ScrollY | ImGuiTableFlags_ScrollX;
    if (ImGui::BeginTable("ENTITY_HIERARCHY", 3, tableFlags))
    {
        ImGui::TableSetupScrollFreeze(0, 1);
        ImGui::TableSetupColumn("Name", ImGuiTableColumnFlags_WidthStretch);
        ImGui::TableSetupColumn("Type", ImGuiTableColumnFlags_WidthFixed, 70.0f);
        ImGui::TableSetupColumn("Active", ImGuiTableColumnFlags_WidthFixed, 40.0f);
        ImGui::TableHeadersRow();

        ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, { 2.0f, 0.0f });
        m_Scene->m_Registry.view<TransformComponent>().each([&](auto entity, auto &nsc)
        {
            DrawEntityNode({ entity, m_Scene.get() });
        });
        ImGui::PopStyleVar();

        if (ImGui::BeginPopupContextWindow("create_entity_context", ImGuiPopupFlags_MouseButtonRight | ImGuiPopupFlags_NoOpenOverItems))
        {
            ShowEntityContextMenu();
            
            ImGui::EndPopup();
        }

        ImGui::EndTable();
    }

    ImGui::EndChild();
    ImGui::End();
}

void SceneHierarchyPanel::EntityPropertiesPanel()
{
    ImGui::Begin("Properties");
    IsScenePropertiesFocused = ImGui::IsWindowFocused();
    if (m_SelectedEntity)
    {
        DrawComponents(m_SelectedEntity);
    }
    ImGui::End();
}

void SceneHierarchyPanel::DrawEntityNode(Entity entity, int index)
{
    if (!entity || (entity.HasParent() && index == 0))
        return;

    ImGuiTreeNodeFlags flags = (m_SelectedEntity == entity ? ImGuiTreeNodeFlags_Selected : 0)
        | ImGuiTreeNodeFlags_OpenOnDoubleClick | ImGuiTreeNodeFlags_OpenOnArrow
        | ImGuiTreeNodeFlags_SpanAvailWidth | ImGuiTreeNodeFlags_SpanFullWidth;


    bool has_children = entity.GetComponent<IDComponent>().HasChild();
    if (!has_children) flags |= ImGuiTreeNodeFlags_Leaf | ImGuiTreeNodeFlags_NoTreePushOnOpen;

    ImGui::TableNextRow();
    ImGui::TableNextColumn();
    bool opened = ImGui::TreeNodeEx((void*)(uint64_t)(uint32_t)entity, flags, entity.GetTag().c_str());

    if (ImGui::IsItemHovered() && ImGui::IsMouseDoubleClicked(0))
    {
        if (entity.HasComponent<UIComponent>())
        {
            auto &ui = entity.GetComponent<UIComponent>();
            UIEditorPanel::GetInstance()->SetContext(m_Scene.get());
            UIEditorPanel::GetInstance()->SetActive(&ui);
        }
    }

    bool is_deleting = false;
    if (!m_Scene->IsRunning())
    {
        ImGui::PushID(reinterpret_cast<void*>(static_cast<uint64_t>(static_cast<uint32_t>(entity))));
        if (ImGui::BeginPopupContextItem(entity.GetTag().c_str()))
        {
            if (ImGui::BeginMenu("Create"))
            {
                if (const Entity e = ShowEntityContextMenu())
                {
                    EntityManager::AddChild(entity, e, m_Scene.get());
                    SetSelectedEntity(e);
                }
                ImGui::EndMenu();
            }
            
            if (ImGui::MenuItem("Delete"))
            {
                DestroyEntity(entity);
                is_deleting = true;
            }
            ImGui::EndPopup();
        }

        if (!is_deleting)
        {
            if (ImGui::BeginDragDropSource())
            {
                ImGui::SetDragDropPayload("ENTITY_SOURCE_ITEM", &entity, sizeof(Entity));
                ImGui::BeginTooltip();
                ImGui::Text("%s %llu", entity.GetTag().c_str(), entity.GetUUID());
                ImGui::EndTooltip();
                ImGui::EndDragDropSource();
            }

            if (ImGui::BeginDragDropTarget())
            {
                if (const ImGuiPayload *payload = ImGui::AcceptDragDropPayload("ENTITY_SOURCE_ITEM"))
                {
                    OGN_CORE_ASSERT(payload->DataSize == sizeof(Entity), "WRONG ENTITY ITEM");
                    Entity src{ *static_cast<entt::entity *>(payload->Data), m_Scene.get() };
                    // the current 'entity' is the target (new parent for src)
                    EntityManager::AddChild(entity, src, m_Scene.get());
                }
                ImGui::EndDragDropTarget();
            }
        }
            
        ImGui::PopID();
    }

    if (!is_deleting && ImGui::IsItemHovered(ImGuiMouseButton_Left) && ImGui::IsMouseReleased(ImGuiMouseButton_Left))
    {
        SetSelectedEntity(entity);
    }

    if (!is_deleting)
    {
        ImGui::PushID(reinterpret_cast<void*>(static_cast<uint64_t>(static_cast<uint32_t>(entity))));
        ImGui::TableNextColumn();
        ImGui::TextWrapped(Utils::EntityTypeToString(entity.GetType()).c_str());
        ImGui::TableNextColumn();
        auto &tc = entity.GetComponent<TransformComponent>();
        ImGui::Checkbox("##Active", &tc.Visible);
        ImGui::PopID();
    }

    if (opened && has_children)
    {
        if (!is_deleting)
        {
            for (const UUID uuid : entity.GetComponent<IDComponent>().Children)
            {
                Entity child_entity = m_Scene->GetEntityWithUUID(uuid);
                DrawEntityNode(child_entity, index + 1);
            }

            /*for (auto e : m_Scene->m_Registry.view<TransformComponent>())
            {
                Entity ent = { e, m_Scene.get() };
                if (ent.GetParentUUID() == entity.GetUUID())
                {
                    DrawEntityNode(ent, index + 1);
                }
            }*/
        }
            
        ImGui::TreePop();
    }
}

void SceneHierarchyPanel::DrawComponents(Entity entity)
{
    if (entity.HasComponent<TagComponent>())
    {
        auto &tag = entity.GetComponent<TagComponent>();
        char buffer[256];
        strncpy(buffer, tag.Tag.c_str(), sizeof(buffer));

        if (ImGui::InputText("##Tag", buffer, sizeof(buffer), ImGuiInputTextFlags_EnterReturnsTrue))
        {
            auto &temp = ComponentsCommand<TagComponent>::GetTempComponent();
            temp = tag;
            tag.Tag = std::string(buffer);
            if (tag.Tag.empty())
            {
                tag.Tag = "'No Name'";
            }

            CommandManager::Instance().ExecuteCommand(
            std::make_unique<ComponentsCommand<TagComponent>>(tag, 
                ComponentsCommand<TagComponent>::GetTempComponent()));
        }
    }

    ImGui::SameLine();
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, { 8.0f, 8.0f });
    ImGui::PushItemWidth(-1);

    if (ImGui::Button("Add", { ImGui::GetContentRegionAvail().x, 0.0f }))
    {
        ImGui::OpenPopup("AddComponent");
    }

    if (ImGui::BeginPopup("AddComponent"))
    {
        std::string search = "Search Component";
        char searchBuffer[256];
        strncpy(searchBuffer, search.c_str(), sizeof(searchBuffer) - 1);

        if (ImGui::InputText("##SearchComponent", searchBuffer, sizeof(searchBuffer)))
        {
            search = std::string(searchBuffer);
        }

        DisplayAddComponentEntry<ScriptComponent>("C# Script");
        DisplayAddComponentEntry<CameraComponent>("Camera");
        DisplayAddComponentEntry<UIComponent>("UI");
        DisplayAddComponentEntry<TextComponent>("Text");
        DisplayAddComponentEntry<ParticleComponent>("Particle");
        DisplayAddComponentEntry<AudioComponent>("Audio Source");
        DisplayAddComponentEntry<AudioListenerComponent>("Audio Listener");

        ImGui::Separator();
        DisplayAddComponentEntry<SpriteRenderer2DComponent>("Sprite Renderer");
        DisplayAddComponentEntry<SpriteAnimationComponent>("Sprite Animation");
        DisplayAddComponentEntry<CircleRendererComponent>("Circle Renderer");
        DisplayAddComponentEntry<Rigidbody2DComponent>("Rigidbody 2D");
        DisplayAddComponentEntry<BoxCollider2DComponent>("Box Collider 2D");
        DisplayAddComponentEntry<CircleCollider2DComponent>("Circle Collider 2D");
        DisplayAddComponentEntry<RevoluteJoint2DComponent>("Revolute Joint 2D");

        ImGui::Separator();
        DisplayAddComponentEntry<DirectionalLightComponent>("Directional Light");
        DisplayAddComponentEntry<PointLightComponent>("Point Light");
        DisplayAddComponentEntry<EnvironmentMapComponent>("Environment Map");
        
        DisplayAddComponentEntry<MeshComponent>("Mesh");
        DisplayAddComponentEntry<RigidbodyComponent>("Rigidbody");
        DisplayAddComponentEntry<BoxColliderComponent>("Box Collider");
        DisplayAddComponentEntry<SphereColliderComponent>("Sphere Collider");
        DisplayAddComponentEntry<CapsuleColliderComponent>("Capsule Collider");
        ImGui::EndPopup();
    }

    ImGui::PopStyleVar();
    ImGui::PopItemWidth();

    DrawComponent<TransformComponent>("Transform", entity, [&](auto &transform)
    {
        UI::DrawVec3Control("Translation", transform.Translation);
        glm::vec3 eulerRotation = glm::degrees(glm::eulerAngles(transform.Rotation));
        if (UI::DrawVec3Control("Rotation", eulerRotation, 1.0f))
        {
            glm::vec3 rotationRadians = glm::radians(eulerRotation);
            transform.Rotation = glm::quat(rotationRadians);
        }
        UI::DrawVec3Control("Scale", transform.Scale, 0.01f, 1.0f);
    });

    DrawComponent<MeshComponent>("Mesh", entity, [&](auto &component)
    {
        ImVec2 buttonSize = { 100.0f, 25.0f };

        // Model Button
        ImGui::Button(component.HModel == 0 ? "Drop Here" : "Loaded", buttonSize);
        if (ImGui::BeginDragDropTarget())
        {
            if (const ImGuiPayload *payload = ImGui::AcceptDragDropPayload("CONTENT_BROWSER_ITEM"))
            {
                AssetHandle handle = *static_cast<AssetHandle *>(payload->Data);
                if (AssetManager::GetAssetType(handle) == AssetType::Mesh)
                {
                    component.HModel = handle;
                    component.blend_space.SetModel(AssetManager::GetAsset<Model>(handle));
                }
                else
                {
                    OGN_CORE_WARN("Wrong asset type!");
                }
            }

            ImGui::EndDragDropTarget();
        }

        if (component.HModel != 0)
        {
            Ref<Model> model = AssetManager::GetAsset<Model>(component.HModel);

            ImGui::SameLine();
            if (ImGui::Button("Blend Space", { 0.0f, buttonSize.y }))
            {
                m_SendData = (void *)&component;
                m_BlendSpacePopUp = true;
            }

            if (!component.blend_space.GetStates().empty())
            {
                ImGui::SliderFloat("Blend X", &component.blend_position.x, component.blend_space.GetMinSize().x, component.blend_space.GetMaxSize().x);
                ImGui::SliderFloat("Blend Y", &component.blend_position.y, component.blend_space.GetMinSize().y, component.blend_space.GetMaxSize().y);
            }

            ImGui::Separator();

            if (ImGui::TreeNode("Meshes"))
            {
                for (auto &mesh : model->GetMeshes())
                {
                    if (ImGui::TreeNode(mesh->name.c_str()))
                    {
                        ImGui::Text("Material Index: %d", mesh->material_index);
                        ImGui::ColorEdit4("Base Color", &mesh->material.buffer_data.base_color.x);
                        ImGui::ColorEdit4("Diffuse Color", &mesh->material.buffer_data.diffuse_color.x);
                        UI::DrawVec2Control("Tiling", mesh->material.buffer_data.tiling_factor);
                        UI::DrawFloatControl("Roughness", &mesh->material.buffer_data.rougness);
                        ImGui::TreePop();
                    }
                }
                ImGui::TreePop();
            }
        }
    });

    DrawComponent<EnvironmentMapComponent>("Environment Map", entity, [](auto &component)
    {
        ImGui::ColorEdit4("Tint Color", &component.tint_color.x);
        UI::DrawFloatControl("Blur", &component.blur_factor, 0.001f, 0.0f, 2.0f);
    });

    DrawComponent<UIComponent>("UI", entity, [](UIComponent &component)
    {
        if(ImGui::Button("Edit"))
        {
            UIEditorPanel::GetInstance()->SetActive(&component);
        }
    });

    DrawComponent<CameraComponent>("Camera", entity, [](auto &component)
    {
        auto &camera = component.Camera;
        UI::DrawCheckbox("Primary", &component.Primary);

        const char* projectionType[2] = { "Perspective", "Orthographic" };
        const char* currentProjectionType = projectionType[static_cast<int>(camera.GetProjectionType())];

        bool is_selected = false;
        if (ImGui::BeginCombo("Projection", currentProjectionType))
        {
            for (int i = 0; i < 2; i++)
            {
                is_selected = currentProjectionType == projectionType[i];
                if (ImGui::Selectable(projectionType[i], is_selected))
                {
                    currentProjectionType = projectionType[i];
                    component.Camera.SetProjectionType(static_cast<ProjectionType>(i));
                }

                if (is_selected)
                    ImGui::SetItemDefaultFocus();
            }
            ImGui::EndCombo();
        }

        const char* aspectRatioType[5] = { "Free", "16/10", "16/9", "21/9", "4/3" };
        const char* currentAspectRatioType = aspectRatioType[static_cast<int>(camera.GetAspectRatioType())];
        
        if (ImGui::BeginCombo("Aspect Ratio", currentAspectRatioType))
        {
            for (int i = 0; i < 5; i++)
            {
                is_selected = currentAspectRatioType == aspectRatioType[i];
                if (ImGui::Selectable(aspectRatioType[i], is_selected))
                {
                    currentAspectRatioType = aspectRatioType[i];
                    camera.SetAspectRatioType(static_cast<AspectRatioType>(i));
                }

                if (is_selected)
                    ImGui::SetItemDefaultFocus();
            }
            ImGui::EndCombo();
        }

        if (component.Camera.GetProjectionType() == ProjectionType::Perspective)
        {
            float perspectiveFov = glm::degrees(camera.GetFOV());
            if (ImGui::DragFloat("FOV", &perspectiveFov, 0.1f, 0.01f, 10000.0f))
            {
                camera.SetFov(glm::radians(perspectiveFov));
            }

            float perspectiveNearClip = camera.GetNear();
            if (ImGui::DragFloat("Near Clip", &perspectiveNearClip, 0.1f))
            {
                camera.SetNear(perspectiveNearClip);
            }

            float perspectiveFarClip = camera.GetFar();
            if (ImGui::DragFloat("Far Clip", &perspectiveFarClip, 0.1f))
            {
                camera.SetFar(perspectiveFarClip);
            }
        }

        if (component.Camera.GetProjectionType() == ProjectionType::Orthographic)
        {
            float orthoScale = camera.GetOrthoScale();
            if (ImGui::DragFloat("Ortho Scale", &orthoScale, 0.1f, 1.0f, 100.0f))
            {
                camera.SetOrthoScale(orthoScale);
            }

            float orthoNearClip = camera.GetOrthoNear();
            if (ImGui::DragFloat("Near Clip", &orthoNearClip, 0.1f, -1.0f, 10.0f))
            {
                camera.SetOrthoNear(orthoNearClip);
            }

            float orthoFarClip = camera.GetOrthoFar();
            if (ImGui::DragFloat("Far Clip", &orthoFarClip, 0.1f, 10.0f, 100.0f))
            {
                camera.SetOrthoFar(orthoFarClip);
            }
        }
    });

    DrawComponent<SpriteAnimationComponent>("Sprite Animation", entity, [](auto &component)
    {
        for (auto anim : component.State->GetStateStorage())
        {
            ImGui::Text(anim.c_str());
        }
    });

    DrawComponent<AudioComponent>("Audio Source", entity, [entity, scene = m_Scene](auto &component)
        {
            std::string label = "None";

            bool is_audio_valid = false;
            ImGui::Text("Audio Source");
            ImGui::SameLine();
            if (component.Audio != 0)
            {
                if (AssetManager::IsAssetHandleValid(component.Audio) && AssetManager::GetAssetType(component.Audio) == AssetType::Audio)
                {
                    const AssetMetadata& metadata = Project::GetActive()->GetEditorAssetManager()->GetMetadata(component.Audio);
                    label = metadata.Filepath.filename().string();
                    is_audio_valid = true;
                }
                else
                {
                    label = "Invalid";
                }
            }

            ImVec2 buttonLabelSize = ImGui::CalcTextSize(label.c_str());
            buttonLabelSize.x += 20.0f;
            const auto buttonLabelWidth = glm::max<float>(100.0f, buttonLabelSize.x);

            ImGui::Button(label.c_str(), { buttonLabelWidth, 0.0f });
            if (ImGui::BeginDragDropTarget())
            {
                if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("CONTENT_BROWSER_ITEM"))
                {
                    AssetHandle handle = *static_cast<AssetHandle*>(payload->Data);
                    if (AssetManager::GetAssetType(handle) == AssetType::Audio)
                    {
                        component.Audio = handle;
                        const AssetMetadata& metadata = Project::GetActive()->GetEditorAssetManager()->GetMetadata(component.Audio);
                        component.Name = metadata.Filepath.filename().string();
                    }
                    else
                    {
                        OGN_CORE_WARN("Wrong asset type!");
                    }
                }
                ImGui::EndDragDropTarget();
            }

            if (is_audio_valid == false)
                return;

            Ref<FmodSound> fmod_sound = AssetManager::GetAsset<FmodSound>(component.Audio);

            if (fmod_sound)
            {
                auto &name = component.Name;
                char buffer[256];
                ImGui::Text("Name");
                ImGui::SameLine();
                strncpy(buffer, name.c_str(), sizeof(buffer) - 1);
                if (ImGui::InputText("##Tag", buffer, sizeof(buffer)))
                {
                    name = std::string(buffer);
                    fmod_sound->SetName(name.c_str());
                }
                    
                UI::DrawCheckbox("Play At Start", &component.PlayAtStart);
                UI::DrawCheckbox("Looping", &component.Looping);
                UI::DrawFloatControl("Volume", &component.Volume, 0.025f, 0.0f, 1.0f, 1.0f);
                UI::DrawFloatControl("Pitch", &component.Pitch, 0.025f, 0.0f, 1.0f, 1.0f);
                UI::DrawFloatControl("Panning", &component.Panning, 0.025f, -1.0f, 1.0f, 0.0f);

                float sizeX = ImGui::GetContentRegionAvail().x;
                if (ImGui::Button("Play", { sizeX, 0.0f })) fmod_sound->Play();
                if (ImGui::Button("Pause", { sizeX, 0.0f })) fmod_sound->Pause();
                if (ImGui::Button("Resume", { sizeX, 0.0f })) fmod_sound->Resume();
                if (ImGui::Button("Stop", { sizeX, 0.0f })) fmod_sound->Stop();
                ImGui::Separator();
            }
        });

    DrawComponent<TextComponent>("Text", entity, [](auto &component) 
        {
            ImGui::Button("DROP FONT", { 80.0f, 30.0f });
            if (ImGui::BeginDragDropTarget())
            {
                if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("CONTENT_BROWSER_ITEM"))
                {
                    AssetHandle handle = *static_cast<AssetHandle *>(payload->Data);
                    if (AssetManager::GetAssetType(handle) == AssetType::Font)
                    {
                        component.FontHandle = handle;
                    }
                }

                ImGui::EndDragDropTarget();
            }

            if (component.FontHandle)
            {
                ImGui::SameLine();
                if (ImGui::Button("X"))
                {
                    component.FontHandle = 0;
                }
            }
                
            if (component.FontHandle != 0)
            {
                ImGui::InputTextMultiline("Text String", &component.TextString);
                ImGui::ColorEdit4("Color", glm::value_ptr(component.Color));
                UI::DrawFloatControl("Kerning", &component.Kerning, 0.01f);
                UI::DrawFloatControl("Line Spacing", &component.LineSpacing, 0.01f);
                UI::DrawCheckbox("Screen Space", &component.ScreenSpace);
            }

        });

    DrawComponent<ParticleComponent>("Particle", entity, [](auto &component)
        {
            float columnWidth = 100.0f;

            ImGui::ColorEdit4("Color Begin", glm::value_ptr(component.ColorBegin));
            ImGui::ColorEdit4("Color End", glm::value_ptr(component.ColorEnd));
            UI::DrawVec3Control("Velocity", component.Velocity, 0.01f, 0.0f, columnWidth);
            UI::DrawVec3Control("Velocity Variation", component.VelocityVariation, 0.01f, 0.0f, columnWidth);
            UI::DrawVec3Control("Rotation", component.Rotation, 0.01f, 0.0f, columnWidth);

            UI::DrawFloatControl("Size Begin", &component.SizeBegin, 0.01f, 0.0f, 1000.0f, 0.5f, columnWidth);
            UI::DrawFloatControl("Size End", &component.SizeEnd, 0.01f, 0.0f, 1000.0f, 0.0f, columnWidth);
            UI::DrawFloatControl("Size Variation", &component.SizeVariation, 0.1f, 0.0f, 1000.0f, 0.3f, columnWidth);
            UI::DrawFloatControl("Z Axis", &component.ZAxis, 0.1f, -1000.0f, 1000.0f, 0.0f, columnWidth);
            UI::DrawFloatControl("Life Time", &component.LifeTime, 0.01f, 0.0f, 1000.0f, 1.0f, columnWidth);
        });

    DrawComponent<SpriteRenderer2DComponent>("Sprite Renderer", entity, [](auto &component)
    {
        ImGui::ColorEdit4("Color", glm::value_ptr(component.Color));

        std::string label = "None";
        if (component.Texture != 0)
        {
            if (AssetManager::IsAssetHandleValid(component.Texture) && AssetManager::GetAssetType(component.Texture) == AssetType::Texture)
            {
                const AssetMetadata& metadata = Project::GetActive()->GetEditorAssetManager()->GetMetadata(component.Texture);
                label = metadata.Filepath.filename().string();
            }
            else
            {
                label = "Invalid";
            }
        }

        UI::DrawButtonWithColumn("Texture", label.c_str(), nullptr, [&]()
        {
            if (ImGui::BeginDragDropTarget())
            {
                if (const ImGuiPayload *payload = ImGui::AcceptDragDropPayload("CONTENT_BROWSER_ITEM"))
                {
                    AssetHandle handle = *static_cast<AssetHandle *>(payload->Data);
                    if (AssetManager::GetAssetType(handle) == AssetType::Texture)
                    {
                        component.Texture = handle;
                        component.UV0 = glm::vec2(0.0f);
                        component.UV1 = glm::vec2(1.0f);
                    }
                    else
                    {
                        OGN_CORE_WARN("Wrong asset type!");
                    }
                }
                else if (const ImGuiPayload *payload = ImGui::AcceptDragDropPayload("SPRITESHEET_ITEM"))
                {
                    SpriteSheetData data = *static_cast<SpriteSheetData *>(payload->Data);
                    component.Texture = data.texture_handle;
                    component.UV0 = data.uv0;
                    component.UV1 = data.uv1;
                }
                ImGui::EndDragDropTarget();
            }

            if (component.Texture != 0)
            {
                ImGui::SameLine();
                if (UI::DrawButton("X"))
                {
                    component.Texture = 0;
                    component.UV0 = glm::vec2(0.0f);
                    component.UV1 = glm::vec2(1.0f);
                }
            }
        });

        if (component.Texture != 0)
        {
            UI::DrawVec2Control("Tilling", component.TillingFactor, 0.025f, 1.0f);
            UI::DrawCheckbox("Flip X", &component.FlipX);
            UI::DrawCheckbox("Flip Y", &component.FlipY);
        }
    });

    DrawComponent<DirectionalLightComponent>("Directional Light", entity, [](DirectionalLightComponent &component)
    {
        glm::vec3 color = { component.color.x, component.color.y, component.color.z };
        if (ImGui::ColorEdit3("Color", &color.x))
        {
            component.color.x = color.x;
            component.color.y = color.y;
            component.color.z = color.z;
        }
    });

    DrawComponent<PointLightComponent>("Point Light", entity, [](PointLightComponent &component)
    {
        ImGui::ColorEdit4("Color", &component.color.x);
        ImGui::ColorEdit4("Intensity", &component.intensity.x);

        glm::vec3 fall_off = glm::vec3(component.falloff);
        if (ImGui::DragFloat3("Fall Off", &fall_off.x, 0.025f, 0.0001f, 1.0f))
        {
            component.falloff.x = fall_off.x;
            component.falloff.y = fall_off.y;
            component.falloff.z = fall_off.z;
        }

    });

    DrawComponent<CircleRendererComponent>("Circle Renderer", entity, [](CircleRendererComponent &component)
    {
        ImGui::ColorEdit4("Color", glm::value_ptr(component.Color));
        ImGui::DragFloat("Thickness", &component.Thickness, 0.025f, 0.0f, 1.0f);
        ImGui::DragFloat("Fade", &component.Fade, 0.025f, 0.0f, 1.0f);
    });

    DrawComponent<Rigidbody2DComponent>("Rigidbody 2D", entity, [](auto &component)
    {
        UI::DrawCheckbox("Enabled", &component.IsEnabled);
        const char* bodyTypeString[] = { "Static", "Dynamic", "Kinematic" };
        const char* currentBodyTypeString = bodyTypeString[static_cast<int>(component.Type)];

        if (ImGui::BeginCombo("Body Type", currentBodyTypeString))
        {
            for (int i = 0; i < 3; i++)
            {
                bool is_selected = currentBodyTypeString == bodyTypeString[i];
                if (ImGui::Selectable(bodyTypeString[i], is_selected))
                {
                    currentBodyTypeString = bodyTypeString[i];
                    component.Type = static_cast<Rigidbody2DComponent::BodyType>(i);
                }
                if (is_selected) ImGui::SetItemDefaultFocus();
            }
            ImGui::EndCombo();
        }

        UI::DrawFloatControl("Gravity Scale", &component.GravityScale, 0.01f);
        UI::DrawFloatControl("Rotational Inertia", &component.RotationalInertia, 0.01f);
        UI::DrawFloatControl("Linear Damping", &component.LinearDamping, 0.025f, 0.0f, 1000.0f);
        UI::DrawFloatControl("Angular Damping", &component.AngularDamping, 0.025f, 0.0f, 1000.0f);
        UI::DrawFloatControl("Mass", &component.Mass, 0.01f);
        UI::DrawVec2Control("Mass Center", component.MassCenter, 0.01f);
        UI::DrawCheckbox("Fixed Rotation", &component.FixedRotation);
        UI::DrawCheckbox("Enable Sleep", &component.EnableSleep);
        UI::DrawCheckbox("Is Awake", &component.IsAwake);
        UI::DrawCheckbox("Is Bullet", &component.IsBullet);
    });

    DrawComponent<BoxCollider2DComponent>("Box Collider 2D", entity, [](auto &component)
        {
            ImGui::DragInt("Group Index", &component.Group, 1.0f, -1, 16, "Group Index %d");

            UI::DrawVec2Control("Offset", component.Offset, 0.01f, 0.0f);
            UI::DrawVec2Control("Size", component.Size, 0.01f, 0.5f);

            UI::DrawCheckbox("Is Sensor", &component.IsSensor);
            UI::DrawFloatControl("Density", &component.Density, 0.025f, 0.0f, 1000.0f, 1.0f);
            UI::DrawFloatControl("Friction", &component.Friction, 0.025f, 0.0f, 1000.0f, 1.0f);
            UI::DrawFloatControl("Restitution", &component.Restitution, 0.025f, 0.0f, 1000.0f, 1.0f);
        });

    DrawComponent<CircleCollider2DComponent>("Circle Collider 2D", entity, [](auto &component)
        {
            ImGui::DragInt("Group Index", &component.Group, 1.0f, -1, 16, "Group Index %d");

            UI::DrawVec2Control("Offset", component.Offset, 0.01f, 0.0f);
            UI::DrawFloatControl("Radius", &component.Radius, 0.01f, 0.5f);

            UI::DrawCheckbox("Is Sensor", &component.IsSensor);
            UI::DrawFloatControl("Density", &component.Density, 0.025f, 0.0f, 1000.0f, 1.0f);
            UI::DrawFloatControl("Friction", &component.Friction, 0.025f, 0.0f, 1000.0f, 1.0f);
            UI::DrawFloatControl("Restitution", &component.Restitution, 0.025f, 0.0f, 1000.0f, 1.0f);
        });

    DrawComponent<RevoluteJoint2DComponent>("Revolute Joint 2D", entity, [&](auto &component)
        {
            std::string label = "Connected Body";

            if (component.ConnectedBodyID != 0)
                label = "Connected";

            ImGui::Button(label.c_str());
            if (ImGui::BeginDragDropTarget())
            {
                if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("ENTITY_SOURCE_ITEM"))
                {
                    OGN_CORE_ASSERT(payload->DataSize == sizeof(Entity), "WRONG ENTITY ITEM");
                    Entity src{ *static_cast<entt::entity*>(payload->Data), m_Scene.get() };
                    if (component.ConnectedBodyID == 0)
                    {
                        UUID uuid = src.GetUUID();
                        component.ConnectedBodyID = uuid;
                    }
                }
                ImGui::EndDragDropTarget();
            }

            if (component.ConnectedBodyID != 0)
            {
                ImGui::SameLine();
                const ImVec2 xLabelSize = ImGui::CalcTextSize("X");
                const float buttonSize = xLabelSize.y + ImGui::GetStyle().FramePadding.y * 2.0f;
                if (ImGui::Button("X", { buttonSize, buttonSize }))
                {
                    component.ConnectedBodyID = 0;
                }
            }
                
            if (component.ConnectedBodyID != 0)
            {
                UI::DrawVec2Control("Anchor A", component.AnchorPoint);
                UI::DrawVec2Control("Anchor B", component.AnchorPointB);
                UI::DrawFloatControl("Upper", &component.UpperAngle, 0.5f, -10000.0f, 10000.0f);
                UI::DrawFloatControl("Lower", &component.LowerAngle, 0.5f, -10000.0f, 10000.0f);

                UI::DrawCheckbox("Collide Connected", &component.CollideConnected);

                UI::DrawCheckbox("Enable Limit", &component.EnableLimit);
                if (component.EnableLimit)
                    UI::DrawFloatControl("Max Torque", &component.MaxMotorTorque, 1.0f, 0.0f, 100000.0f);
                UI::DrawCheckbox("Enable Motor", &component.EnableMotor);
                if (component.EnableMotor)
                    UI::DrawFloatControl("Motor Speed", &component.MotorSpeed, 1.0f, 0.0f, 100000.0f);
                UI::DrawCheckbox("Enable Spring", &component.EnableSpring);
                if (component.EnableSpring)
                {
                    UI::DrawFloatControl("Damping Ratio", &component.SpringDampingRatio);
                }
            }
                
        });
    DrawComponent<RigidbodyComponent>("Rigidbody", entity, [scene = m_Scene](auto &component)
    {
        const char *motionQuality[2] = { "Discrete", "LinearCast" };
        const char *currentMotionQuality = motionQuality[static_cast<int>(component.MotionQuality)];

        bool is_selected = false;
        if (ImGui::BeginCombo("Motion Quality", currentMotionQuality))
        {
            for (int i = 0; i < 2; i++)
            {
                is_selected = currentMotionQuality == motionQuality[i];
                if (ImGui::Selectable(motionQuality[i], is_selected))
                {
                    currentMotionQuality = motionQuality[i];
                    component.MotionQuality = static_cast<RigidbodyComponent::EMotionQuality>(i);
                }

                if (is_selected)
                {
                    ImGui::SetItemDefaultFocus();
                }
            }
            ImGui::EndCombo();
        }

        UI::DrawCheckbox("UseGravity", &component.UseGravity);

        static float coloumnWidth = 100.0f;
        UI::DrawCheckbox3("Rotate", &component.RotateX, &component.RotateY, &component.RotateZ);
        UI::DrawCheckbox3("Translate", &component.MoveX, &component.MoveY, &component.MoveZ);

        UI::DrawCheckbox("Static", &component.IsStatic);
        UI::DrawCheckbox("Allow Sleeping", &component.AllowSleeping);
        if (UI::DrawFloatControl("Gravity Factor", &component.GravityFactor, 0.05f, 0.0f, 1000.0f, 1.0f))
        {
            if (scene->IsRunning())
            {
                component.SetGravityFactor(component.GravityFactor);
            }
        }
            
        if (UI::DrawFloatControl("Mass", &component.Mass, 0.05f, 0.0f, 1000.0f, 1.0f))
        {
            if (scene->IsRunning())
            {
                component.SetMass(component.Mass);
            }
        }
        if (UI::DrawVec3Control("Offset", component.Offset, 0.025f, 0.0f))
        {
            if (scene->IsRunning())
            {
                component.SetOffset(component.Offset);
            }
        }
        if (UI::DrawVec3Control("Center Mass", component.CenterMass, 0.025f, 0.0f))
        {
            if (scene->IsRunning())
            {
                component.SetCenterMass(component.CenterMass);
            }
        }
    });

    DrawComponent<BoxColliderComponent>("Box Collider", entity, [](auto &component)
    {
        UI::DrawVec3Control("Scale", component.Scale, 0.025f, 1.0f);
        UI::DrawFloatControl("Friction", &component.Friction, 0.025f, 0.0f, 1000.0f, 0.6f);
        UI::DrawFloatControl("Restitution", &component.Restitution, 0.025f, 0.0f, 1000.0f, 0.6f);
    });

    DrawComponent<SphereColliderComponent>("Sphere Collider", entity, [](auto &component)
    {
        UI::DrawFloatControl("Radius", &component.Radius, 0.025f, 0.0f, 10.0f, 1.0f);
        UI::DrawFloatControl("Friction", &component.Friction, 0.025f, 0.0f, 1000.0f, 0.5f);
        UI::DrawFloatControl("Restitution", &component.Restitution, 0.025f, 0.0f, 1000.0f, 0.0f);
    });

    DrawComponent<CapsuleColliderComponent>("Capsule Collider", entity, [](auto &component)
    {
        UI::DrawFloatControl("Half Height", &component.HalfHeight, 0.025f, 0.0f, 10.0f, 1.0f);
        UI::DrawFloatControl("Radius", &component.Radius, 0.025f, 0.0f, 10.0f, 1.0f);
        UI::DrawFloatControl("Friction", &component.Friction, 0.025f, 0.0f, 1000.0f, 0.5f);
        UI::DrawFloatControl("Restitution", &component.Restitution, 0.025f, 0.0f, 1000.0f, 0.0f);
    });

    DrawComponent<ScriptComponent>("C# Script", entity, [entity, scene = m_Scene](auto &component) mutable
        {
            bool scriptClassExist = ScriptEngine::EntityClassExists(component.ClassName);
            bool is_selected = false;

            if (!scriptClassExist)
            {
                ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.9f, 0.2f, 0.2f, 1.0f));
            }

            auto scriptStorage = ScriptEngine::GetScriptClassStorage();
            std::string currentScriptClasses = component.ClassName;

            // drop-down
            if (ImGui::BeginCombo("Script Class", currentScriptClasses.c_str()))
            {
                for (int i = 0; i < scriptStorage.size(); i++)
                {
                    is_selected = currentScriptClasses == scriptStorage[i];
                    if (ImGui::Selectable(scriptStorage[i].c_str(), is_selected))
                    {
                        currentScriptClasses = scriptStorage[i];
                        component.ClassName = scriptStorage[i];
                    }
                    if (is_selected) ImGui::SetItemDefaultFocus();
                }
                ImGui::EndCombo();
            }

            if (ImGui::Button("Detach"))
            {
                component.ClassName = "Detached";
                is_selected = false;
            }

            bool detached = component.ClassName == "Detached";

            // fields
            bool isRunning = scene->IsRunning();

            if (isRunning && !detached)
            {
                std::shared_ptr<ScriptInstance> scriptInstance = ScriptEngine::GetEntityScriptInstance(entity.GetUUID());
                if (scriptInstance)
                {
                    auto fields = scriptInstance->GetScriptClass()->GetFields();
                    for (const auto &[name, field] : fields)
                    {
                        switch (field.Type)
                        {
                        case ScriptFieldType::Float:
                        {
                            float data = scriptInstance->GetFieldValue<float>(name);
                            if (UI::DrawFloatControl(name.c_str(), &data, 0.1f))
                            {
                                scriptInstance->SetFieldValue<float>(name, data);
                            }
                            break;
                        }
                        case ScriptFieldType::Int:
                        {
                            int data = scriptInstance->GetFieldValue<int>(name);
                            if (UI::DrawIntControl(name.c_str(), &data, 1))
                            {
                                scriptInstance->SetFieldValue<int>(name, data);
                            }
                            break;
                        }
                        case ScriptFieldType::Vector2:
                        {
                            glm::vec2 data = scriptInstance->GetFieldValue<glm::vec2>(name);
                            if (UI::DrawVec2Control(name.c_str(), data, 0.1f))
                            {
                                scriptInstance->SetFieldValue<glm::vec2>(name, data);
                            }
                            break;
                        }
                        case ScriptFieldType::Vector3:
                        {
                            glm::vec3 data = scriptInstance->GetFieldValue<glm::vec3>(name);
                            if (UI::DrawVec3Control(name.c_str(), data, 0.1f))
                            {
                                scriptInstance->SetFieldValue<glm::vec3>(name, data);
                            }
                            break;
                        }
                        case ScriptFieldType::Vector4:
                        {
                            glm::vec4 data = scriptInstance->GetFieldValue<glm::vec4>(name);
                            if (UI::DrawVec4Control(name.c_str(), data, 0.1f))
                            {
                                scriptInstance->SetFieldValue<glm::vec4>(name, data);
                            }
                            break;
                        }
                        case ScriptFieldType::Entity:
                            uint64_t uuid = scriptInstance->GetFieldValue<uint64_t>(name);
                            Entity e = scene->GetEntityWithUUID(uuid);
                            if (e)
                            {
                                UI::DrawButtonWithColumn(name.c_str(), e.GetTag().c_str(), nullptr, [&]()
                                {
                                    if (ImGui::IsItemHovered())
                                    {
                                        ImGui::BeginTooltip();
                                        ImGui::Text("%llu", uuid);
                                        ImGui::EndTooltip();
                                    }
                                });
                            }
                            
                            break;
                        }
                    }

                }
            }
            else if (!isRunning && scriptClassExist && !detached)
            {
                // !IsRunning
                std::shared_ptr<ScriptClass> entityClass = ScriptEngine::GetEntityClassesByName(component.ClassName);
                if (entityClass)
                {
                    const auto &fields = entityClass->GetFields();
                    auto &entityFields = ScriptEngine::GetScriptFieldMap(entity);
                    for (const auto &[name, field] : fields)
                    {
                        if (entityFields.find(name) != entityFields.end())
                        {
                            ScriptFieldInstance &scriptField = entityFields.at(name);

                            switch (field.Type)
                            {
                            case ScriptFieldType::Float:
                            {
                                float data = scriptField.GetValue<float>();
                                if (UI::DrawFloatControl(name.c_str(), &data, 0.1f))
                                {
                                    scriptField.SetValue<float>(data);
                                }
                                break;
                            }
                            case ScriptFieldType::Int:
                            {
                                int data = scriptField.GetValue<int>();
                                if (UI::DrawIntControl(name.c_str(), &data))
                                {
                                    scriptField.SetValue<int>(data);
                                }
                                break;
                            }
                            case ScriptFieldType::Vector2:
                            {
                                glm::vec2 data = scriptField.GetValue<glm::vec3>();
                                if (UI::DrawVec2Control(name.c_str(), data, 0.1f))
                                {
                                    scriptField.SetValue<glm::vec2>(data);
                                }
                                break;
                            }
                            case ScriptFieldType::Vector3:
                            {
                                glm::vec3 data = scriptField.GetValue<glm::vec3>();
                                if (UI::DrawVec3Control(name.c_str(), data, 0.1f))
                                {
                                    scriptField.SetValue<glm::vec3>(data);
                                }
                                break;
                            }
                            case ScriptFieldType::Vector4:
                            {
                                glm::vec4 data = scriptField.GetValue<glm::vec4>();
                                if (UI::DrawVec4Control(name.c_str(), data, 0.1f))
                                {
                                    scriptField.SetValue<glm::vec4>(data);
                                }
                                break;
                            }
                            case ScriptFieldType::Entity:
                            {
                                uint64_t uuid = scriptField.GetValue<uint64_t>();
                                std::string label = "Drag Here";
                                if (uuid)
                                {
                                    Entity e = scene->GetEntityWithUUID(uuid);
                                    if (e)
                                    {
                                        label = e.GetTag();
                                    }
                                }

                                UI::DrawButtonWithColumn(name.c_str(), label.c_str(), nullptr, [&]()
                                    {
                                        if (ImGui::BeginDragDropTarget())
                                        {
                                            if (const ImGuiPayload *payload = ImGui::AcceptDragDropPayload("ENTITY_SOURCE_ITEM"))
                                            {
                                                OGN_CORE_ASSERT(payload->DataSize == sizeof(Entity), "WRONG ENTITY ITEM");
                                                if (payload->DataSize == sizeof(Entity))
                                                {
                                                    Entity src{ *static_cast<entt::entity *>(payload->Data), scene.get() };
                                                    uint64_t id = (uint64_t)src.GetUUID();
                                                    scriptField.Field.Type = ScriptFieldType::Entity;
                                                    scriptField.SetValue<uint64_t>(id);
                                                }
                                            }
                                            ImGui::EndDragDropTarget();
                                        }

                                        if (ImGui::IsItemHovered())
                                        {
                                            ImGui::BeginTooltip();

                                            if (uuid)
                                                ImGui::Text("%llu", uuid);
                                            else
                                                ImGui::Text("Null Entity!");

                                            ImGui::EndTooltip();
                                        }

                                        ImGui::SameLine();
                                        if (UI::DrawButton("X"))
                                        {
                                            scriptField.SetValue<uint64_t>(0);
                                        }
                                    });
                                break;
                            }
                            }
                        }
                        else
                        {
                            ScriptFieldInstance &fieldInstance = entityFields[name];
                            switch (field.Type)
                            {
                            case ScriptFieldType::Float:
                            {
                                float data = 0.0f;
                                if (UI::DrawFloatControl(name.c_str(), &data, 0.1f))
                                {
                                    fieldInstance.Field = field;
                                    fieldInstance.SetValue<float>(data);
                                }
                                break;
                            }
                            case ScriptFieldType::Int:
                            {
                                int data = 0;
                                if (ImGui::DragInt(name.c_str(), &data))
                                {
                                    fieldInstance.Field = field;
                                    fieldInstance.SetValue<int>(data);
                                }
                                break;
                            }
                            case ScriptFieldType::Vector2:
                            {
                                glm::vec2 data(0.0f);
                                if (UI::DrawVec2Control(name.c_str(), data, 0.1f))
                                {
                                    fieldInstance.Field = field;
                                    fieldInstance.SetValue<glm::vec2>(data);
                                }
                                break;
                            }
                            case ScriptFieldType::Vector3:
                            {
                                glm::vec3 data(0.0f);
                                if (UI::DrawVec3Control(name.c_str(), data, 0.1f))
                                {
                                    fieldInstance.Field = field;
                                    fieldInstance.SetValue<glm::vec3>(data);
                                }
                                break;
                            }
                            case ScriptFieldType::Vector4:
                            {
                                glm::vec4 data(0.0f);
                                if (UI::DrawVec4Control(name.c_str(), data, 0.1f))
                                {
                                    fieldInstance.Field = field;
                                    fieldInstance.SetValue<glm::vec4>(data);
                                }
                                break;
                            }
                            case ScriptFieldType::Entity:
                            {
                                UI::DrawButtonWithColumn(name.c_str(), "Drag Here", nullptr, [&]()
                                    {
                                        if (ImGui::BeginDragDropTarget())
                                        {
                                            if (const ImGuiPayload *payload = ImGui::AcceptDragDropPayload("ENTITY_SOURCE_ITEM"))
                                            {
                                                OGN_CORE_ASSERT(payload->DataSize == sizeof(Entity), "WRONG ENTITY ITEM");
                                                if (payload->DataSize == sizeof(Entity))
                                                {
                                                    Entity src{ *static_cast<entt::entity *>(payload->Data), scene.get() };
                                                    fieldInstance.Field = field;
                                                    fieldInstance.SetValue<uint64_t>(src.GetUUID());
                                                }
                                            }
                                            ImGui::EndDragDropTarget();
                                        }

                                        if (ImGui::IsItemHovered())
                                        {
                                            ImGui::BeginTooltip();
                                            ImGui::Text("Null Entity!");
                                            ImGui::EndTooltip();
                                        }

                                        ImGui::SameLine();
                                        if (UI::DrawButton("X"))
                                        {
                                            fieldInstance.Field = field;
                                            fieldInstance.SetValue<uint64_t>(0);
                                        }
                                    });
                                break;
                            }
                            default:
                                break;
                            }
                        }
                    }
                }
            }

            if (!scriptClassExist)
                ImGui::PopStyleColor();
        });

    DrawComponent<AudioListenerComponent>("Audio Listener", entity, [](auto &component)
        {
            UI::DrawCheckbox("Enable", &component.Enable);
        });
}

template<typename T>
bool SceneHierarchyPanel::DisplayAddComponentEntry(const std::string& entry_name)
{
    if (ImGui::MenuItem(entry_name.c_str()))
    {
        m_SelectedEntity.AddOrReplaceComponent<T>();
        ImGui::CloseCurrentPopup();
        return true;
    }
    return false;
}

template<typename T, typename UIFunction>
void SceneHierarchyPanel::DrawComponent(const std::string &name, Entity entity, UIFunction uiFunction)
{
    constexpr ImGuiTreeNodeFlags tree_node_flags = ImGuiTreeNodeFlags_DefaultOpen
        | ImGuiTreeNodeFlags_Framed | ImGuiTreeNodeFlags_SpanAvailWidth
        | ImGuiTreeNodeFlags_AllowItemOverlap | ImGuiTreeNodeFlags_FramePadding;

    if (entity.HasComponent<T>())
    {
        auto &component = entity.GetComponent<T>();
        const ImVec2 content_region_available = ImGui::GetContentRegionAvail();

        ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2 { 0.5f, 2.0f });
        ImGui::Separator();
        const bool open = ImGui::TreeNodeEx(reinterpret_cast<void *>(typeid(T).hash_code()), tree_node_flags, name.c_str());
        ImGui::PopStyleVar();

        ImGui::SameLine(content_region_available.x - 24.0f);
        const ImTextureID tex_id = reinterpret_cast<void *>(static_cast<uintptr_t>(EditorLayer::Get().m_UITextures.at("plus")->GetID()));
        if (ImGui::ImageButton("component_more_button", tex_id, { 14.0f, 14.0f }))
            ImGui::OpenPopup("Component Settings");

        bool component_removed = false;
        if (ImGui::BeginPopup("Component Settings"))
        {
            if (ImGui::MenuItem("Remove component"))
                component_removed = true;

            ImGui::EndPopup();
        }

        if (open)
        {
            uiFunction(component);
            ImGui::TreePop();
        }

        if (component_removed)
            entity.RemoveComponent<T>();
    }
}

}
