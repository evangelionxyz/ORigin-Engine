﻿// Copyright (c) Evangelion Manuhutu | ORigin Engine

#include "pch.h"
#include "Scene.h"

#include "Origin/Audio/AudioEngine.h"
#include "Origin/Audio/AudioSource.h"
#include "Origin/Audio/FmodSound.h"
#include "Origin/Audio/FmodAudio.h"
#include "Origin/Profiler/Profiler.h"
#include "Origin/Animation/Animation.h"
#include "Origin/Physics/2D/Physics2D.h"
#include "Origin/Renderer/Renderer.h"
#include "Origin/Renderer/2D/Renderer2D.hpp"
#include "Origin/Renderer/Model/Model.hpp"
#include "Origin/Renderer/Lighting/Lighting.hpp"
#include "Origin/Scripting/ScriptEngine.h"
#include "Origin/Asset/AssetManager.h"
#include "Origin/Core/Log.h"
#include "Origin/Core/Input.h"
#include "EntityManager.h"
#include "Origin/Math/Math.hpp"
#include "Entity.h"
#include "ScriptableEntity.h"

#include "Origin/Physics/Physics.hpp"
#include "Origin/Physics/Jolt/JoltScene.hpp"
#include "Origin/Physics/PhysX/PhysXScene.hpp"

#include <glad/glad.h>

#include <ktx.h>

#include <glm/glm.hpp>

namespace origin {
class Model;

Scene::Scene()
{
    OGN_PROFILER_SCENE();

    switch (Physics::GetAPI())
    {
    case PhysicsAPI::Jolt:
    {
        m_Physics = CreateRef<JoltScene>(this);
        break;
    }
    case PhysicsAPI::PhysX:
    {
        m_Physics = CreateRef<PhysXScene>(this);
        break;
    }
    }

    m_Physics2D = CreateRef<Physics2D>(this);
    m_UIRenderer = CreateRef<UIRenderer>();
}

Scene::~Scene()
{
}

Ref<Scene> Scene::Copy(const Ref<Scene> &other)
{
    OGN_PROFILER_SCENE();

    auto newScene = CreateRef<Scene>();
    newScene->m_Name = other->m_Name;

    newScene->m_ViewportWidth = other->m_ViewportWidth;
    newScene->m_ViewportHeight = other->m_ViewportHeight;

    entt::registry &srcSceneRegistry = other->m_Registry;
    entt::registry &dstSceneRegistry = newScene->m_Registry;
    std::vector<std::pair<UUID, entt::entity>> enttStorage;
    auto newEntity = Entity();

    // create entities in new scene
    const auto &idView = srcSceneRegistry.view<IDComponent>();
    for (auto e : idView)
    {
        const auto &idc = srcSceneRegistry.get<IDComponent>(e);
        const auto &name = srcSceneRegistry.get<TagComponent>(e).Tag;
        newEntity = EntityManager::CreateEntityWithUUID(idc.ID, name, idc.Type, newScene.get());

        auto &eIDC = newEntity.GetComponent<IDComponent>();
        eIDC.Parent = idc.Parent;
        eIDC.Children = std::move(idc.Children);

        enttStorage.push_back({ idc.ID, static_cast<entt::entity>(newEntity) });
    }

    EntityManager::CopyComponent(AllComponents{}, dstSceneRegistry, srcSceneRegistry, enttStorage);
    return newScene;
}

Entity Scene::GetEntityWithUUID(UUID uuid)
{
    OGN_PROFILER_SCENE();

    for (auto e : m_EntityStorage)
    {
        if (e.first == uuid)
        {
            return { e.second, this };
        }
    }

    return Entity{ entt::null, nullptr };
}

Entity Scene::FindEntityByName(std::string_view name)
{
    OGN_PROFILER_SCENE();

    for (const auto view = m_Registry.view<TagComponent>(); auto entity : view)
    {
        const TagComponent &tc = view.get<TagComponent>(entity);
        if (tc.Tag == name)
            return { entity, this };
    }

    return Entity();
}

void Scene::PreRender(const Camera &camera, Timestep ts)
{
    OGN_PROFILER_FUNCTION();

    // bind lighting
    LightingManager::GetInstance()->Bind();
    for (auto &e: m_Registry.view<TransformComponent>())
    {
        Entity entity{ e, this };
        TransformComponent &transform_comp = m_Registry.get<TransformComponent>(e);
        IDComponent &id_comp = m_Registry.get<IDComponent>(e);

        if (entity.HasComponent<DirectionalLightComponent>())
        {
            DirectionalLightComponent &dir_light_comp = m_Registry.get<DirectionalLightComponent>(e);
            Ref<DirectionalLight> dir_light = std::static_pointer_cast<DirectionalLight>(dir_light_comp.Light);
            dir_light_comp.direction = glm::vec4(eulerAngles(transform_comp.WorldRotation), 1.0f);
            dir_light->data.direction = dir_light_comp.direction;
            dir_light->data.color = dir_light_comp.color;
            dir_light->Bind();
        }

        if (entity.HasComponent<PointLightComponent>())
        {
            PointLightComponent &pointlight_comp = m_Registry.get<PointLightComponent>(e);
            Ref<PointLight> light = std::static_pointer_cast<PointLight>(pointlight_comp.Light);
            light->data.position = glm::vec4(transform_comp.WorldTranslation, 1.0f);
            light->data.color = pointlight_comp.color;
            light->data.intensity = pointlight_comp.intensity;
            light->data.falloff = pointlight_comp.falloff;
            LightingManager::GetInstance()->UpdatePointLight(light->index, light->data);
        }

        if (id_comp.Parent)
        {
            if (auto parent = GetEntityWithUUID(id_comp.Parent))
            {
                auto &ptc = parent.GetComponent<TransformComponent>();
                glm::vec3 rotated_local_pos = ptc.WorldRotation * transform_comp.Translation;
                transform_comp.WorldTranslation = rotated_local_pos + ptc.WorldTranslation;
                transform_comp.WorldRotation = ptc.WorldRotation * transform_comp.Rotation;
                transform_comp.WorldScale = transform_comp.Scale;
            }
        }
        else
        {
            transform_comp.WorldTranslation = transform_comp.Translation;
            transform_comp.WorldRotation = transform_comp.Rotation;
            transform_comp.WorldScale = transform_comp.Scale;
        }
    }

    std::unordered_set<UUID> updated_models;
    const auto &view = m_Registry.view<TransformComponent>();
    for (auto e : view)
    {
        Entity entity{ e, this };
        TransformComponent &transform_comp = view.get<TransformComponent>(e);

        if (!transform_comp.Visible)
            continue;

        if (entity.HasComponent<MeshComponent>())
        {
            MeshComponent &mesh_comp = m_Registry.get<MeshComponent>(e);
            if (mesh_comp.HModel == 0)
                continue;

            Shader *shader = Renderer::GetShader("SkinnedMesh").get();
            Ref<Model> model = AssetManager::GetAsset<Model>(mesh_comp.HModel);

            // UPDATING ANIMATION
            if (model->HasAnimations() && !updated_models.contains(mesh_comp.HModel))
            {
                if (!mesh_comp.blend_space.GetStates().empty())
                {
                    mesh_comp.blend_space.BlendAnimations(mesh_comp.blend_position, ts, mesh_comp.animation_speed);
                }
                else
                {
                    model->UpdateAnimation(ts, mesh_comp.AnimationIndex);
                }
            }
        }
    }

    m_UIRenderer->RenderFramebuffer();
}

void Scene::PostRender(const Camera &camera, Timestep ts)
{
    LightingManager::GetInstance()->Unbind();
}

void Scene::OnGuiRender()
{
}

void Scene::Update(Timestep ts)
{
    for (const auto &view = m_Registry.view<TransformComponent>(); auto e : view)
    {
        Entity entity = { e, this };
        auto &tc = entity.GetComponent<TransformComponent>();
        if (entity.HasComponent<SpriteAnimationComponent>())
        {
            if (const auto &ac = entity.GetComponent<SpriteAnimationComponent>(); ac.State->IsCurrentAnimationExists())
            {
                ac.State->OnUpdateRuntime(ts);
            }
        }

        if (entity.HasComponent<ParticleComponent>())
        {
            auto &pc = entity.GetComponent<ParticleComponent>();
            pc.Particle.OnUpdate(ts);
        }

        if (entity.HasComponent<AudioComponent>())
        {
            auto &ac = entity.GetComponent<AudioComponent>();
            if (Ref<FmodSound> fmod_sound = AssetManager::GetAsset<FmodSound>(ac.Audio))
            {
                fmod_sound->SetVolume(ac.Volume);
                fmod_sound->SetPitch(ac.Pitch);
                fmod_sound->SetPan(ac.Panning);
            }
        }

        if (entity.HasComponent<AudioListenerComponent>())
        {
            auto &al = entity.GetComponent<AudioListenerComponent>();
            al.Listener.SetEnable(al.Enable);
            if (al.Enable) al.Listener.Set(tc.Translation, glm::vec3(0.0f), tc.GetForward(), tc.GetUp());
        }
    }
}

void Scene::UpdateScripts(Timestep ts)
{
    // Update Scripts
    m_Registry.view<ScriptComponent>().each([this, time = ts](auto entityID, auto &sc)
    {
        const Entity entity{ entityID, this };
        ScriptEngine::OnUpdateEntity(entity, time);
    });

    m_Registry.view<NativeScriptComponent>().each([this, time = ts](auto entityID, auto &nsc)
    {
        nsc.Instance->OnUpdate(time);
    });
}

void Scene::UpdatePhysics(Timestep ts) const
{
    if (m_Physics)
        m_Physics->Simulate(ts);

    m_Physics2D->Simulate(ts);
}

void Scene::OnUpdateRuntime(const Timestep ts)
{
    OGN_PROFILER_SCENE();

    if (!m_Paused || m_StepFrames-- > 0)
    {
        UpdateScripts(ts);
        Update(ts);
        UpdatePhysics(ts);
    }

    auto cameraView = m_Registry.view<CameraComponent, TransformComponent>();
    for (auto entity : cameraView)
    {
        auto [tc, cc] = cameraView.get<TransformComponent, CameraComponent>(entity);
        if (cc.Primary)
        {
            cc.Camera.SetTransform(tc.GetTransform());
            RenderScene(cc.Camera);
            break;
        }
    }
}

void Scene::OnRuntimeStart()
{
    OGN_PROFILER_SCENE();
    m_Running = true;
    ScriptEngine::SetSceneContext(this);

    if (m_Physics) 
        m_Physics->StartSimulation();
    m_Physics2D->OnSimulationStart();

    for (auto [e, sc] : m_Registry.view<ScriptComponent>().each())
    {
        Entity entity = { e, this };
        ScriptEngine::OnCreateEntity(entity);
    }

    for (auto [e, nsc] : m_Registry.view<NativeScriptComponent>().each())
    {
        if (!nsc.Instance)
        {
            nsc.Instance = nsc.InstantiateScript();
            nsc.Instance->m_Entity = { e, this };
        }
    };

    for (auto [e, ac] : m_Registry.view<AudioComponent>().each())
    {
        const Ref<FmodSound> &audio = AssetManager::GetAsset<FmodSound>(ac.Audio);
        if (ac.PlayAtStart)
        {
            audio->Play();
        }
    }

#if 0
    m_Registry.view<UIComponent>().each([this](entt::entity e, UIComponent ui)
    {
        auto cam = GetPrimaryCameraEntity();
        auto cc = cam.GetComponent<CameraComponent>();
        if (cc.Primary)
        {
            m_UIRenderer->AddUI(ui);
            cc.Camera.SetViewportSize(m_ViewportWidth, m_ViewportHeight);
            const glm::ivec2 &vp = cc.Camera.GetViewportSize();
            const glm::vec2 &ortho = cc.Camera.GetOrthoSize();
            m_UIRenderer->CreateFramebuffer(vp.x, vp.y, ortho.x, ortho.y);
        }
    });
#endif
}

void Scene::OnRuntimeStop()
{
    OGN_PROFILER_SCENE();

    m_Running = false;
    ScriptEngine::ClearSceneContext();
    m_Registry.view<NativeScriptComponent>().each([this](auto entity, auto &nsc)
    {
        nsc.DestroyScript(&nsc);
    });

    const auto &audioView = m_Registry.view<AudioComponent>();

    for (auto &e : audioView)
    {
        auto &ac = audioView.get<AudioComponent>(e);
        if (const Ref<FmodSound> &fmod_sound = AssetManager::GetAsset<FmodSound>(ac.Audio))
        {
            fmod_sound->Stop();
        }
    }

    if (m_Physics)
        m_Physics->StopSimulation();
    m_Physics2D->OnSimulationStop();

    m_UIRenderer->Unload();
}

void Scene::OnUpdateEditor(const Camera &camera, Timestep ts, entt::entity selectedID)
{
    Update(ts);
    RenderScene(camera);
}

void Scene::OnUpdateSimulation(const Camera &camera, Timestep ts, entt::entity selectedID)
{
    if (!m_Paused || m_StepFrames-- > 0)
    {
        UpdateScripts(ts);
        Update(ts);
        UpdatePhysics(ts);
    }

    RenderScene(camera);
}

void Scene::RenderScene(const Camera &camera)
{
    OGN_PROFILER_RENDERING();

    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);
    glDepthMask(GL_TRUE);
    
    Renderer2D::Begin();
    const auto &view = m_Registry.view<TransformComponent>();
    for (auto e : view)
    {
        Entity entity{ e, this };
        TransformComponent &transform_comp = view.get<TransformComponent>(e);

        if (!transform_comp.Visible)
            continue;

        // 2D Quads
        if (entity.HasComponent<SpriteRenderer2DComponent>())
        {
            auto &src = entity.GetComponent<SpriteRenderer2DComponent>();
            if (entity.HasComponent<SpriteAnimationComponent>())
            {
                auto &ac = entity.GetComponent<SpriteAnimationComponent>();
                if (ac.State->IsCurrentAnimationExists())
                {
                    if (ac.State->GetAnimation()->HasFrame())
                    {
                        auto &anim = ac.State->GetAnimation();
                        src.Texture = anim->GetCurrentFrame().Handle;
                        src.UV0 = anim->GetCurrentFrame().UV0;
                        src.UV1 = anim->GetCurrentFrame().UV1;
                    }
                }
            }
            Renderer2D::DrawSprite(transform_comp.GetTransform(), src);
        }

        if (entity.HasComponent<CircleRendererComponent>())
        {
            auto &cc = m_Registry.get<CircleRendererComponent>(e);
            Renderer2D::DrawCircle(transform_comp.GetTransform(), cc.Color, cc.Thickness, cc.Fade);
        }

        // Particles
        if (entity.HasComponent<ParticleComponent>())
        {
            auto &pc = m_Registry.get<ParticleComponent>(e);
            for (int i = 0; i < 5; i++)
            {
                pc.Particle.Emit(pc,
                    { transform_comp.WorldTranslation.x, transform_comp.WorldTranslation.y, transform_comp.WorldTranslation.z + pc.ZAxis },
                    transform_comp.WorldScale, pc.Rotation);
            }

            pc.Particle.OnRender();
        }

        // Text
        if (entity.HasComponent<TextComponent>())
        {
            auto &text = m_Registry.get<TextComponent>(e);
            glm::mat4 transform = glm::mat4(1.0f);
            glm::mat4 invertedCamTransform = glm::mat4(1.0f);

            if (text.ScreenSpace)
            {
                if (camera.IsPerspective())
                {
                    if (Entity primary_cam = GetPrimaryCameraEntity())
                    {
                        const auto &cc = primary_cam.GetComponent<CameraComponent>().Camera;
                        const auto &ccTC = primary_cam.GetComponent<TransformComponent>();
                        const float ratio = cc.GetAspectRatio();
                        glm::vec2 scale = glm::vec2(transform_comp.WorldScale.x, transform_comp.WorldScale.y * ratio);
                        transform = glm::translate(glm::mat4(1.0f), { transform_comp.WorldTranslation.x, transform_comp.WorldTranslation.y, 0.0f })
                            * glm::toMat4(ccTC.Rotation)
                            * glm::scale(glm::mat4(1.0f), { transform_comp.WorldScale.x, transform_comp.WorldScale.y * ratio, 0.0 });

                        invertedCamTransform = glm::inverse(cc.GetViewProjection());
                    }
                }
                else
                {
                    const float ratio = camera.GetAspectRatio();
                    transform = glm::scale(transform_comp.GetTransform(), { transform_comp.WorldScale.x, transform_comp.WorldScale.y * ratio, 0.0 });
                    invertedCamTransform = glm::inverse(camera.GetViewProjection());
                }

                transform = invertedCamTransform * transform;
            }
            else
            {
                glm::vec3 center_offset = glm::vec3(text.Size.x / 2.0f, -text.Size.y / 2.0f + 1.0f, 0.0f);
                glm::vec3 scaled_offset = transform_comp.WorldScale * center_offset;
                glm::vec3 rotated_offset = glm::toMat3(transform_comp.WorldRotation) * scaled_offset;

                text.Position = transform_comp.WorldTranslation - rotated_offset;

                transform = glm::translate(glm::mat4(1.0f), text.Position)
                    * glm::toMat4(transform_comp.WorldRotation)
                    * glm::scale(glm::mat4(1.0f), transform_comp.WorldScale);
            }

            if (text.FontHandle != 0)
            {
                Renderer2D::DrawString(text.TextString, transform, text);
            }
        }

        if (entity.HasComponent<MeshComponent>())
        {
            MeshComponent &mesh_component = m_Registry.get<MeshComponent>(e);
            if (mesh_component.HModel == 0)
                continue;

            Shader *shader = Renderer::GetShader("SkinnedMesh").get();
            shader->Enable();
            Ref<Model> model = AssetManager::GetAsset<Model>(mesh_component.HModel);
            shader->SetBool("uhas_animation", model->HasAnimations());
            if (model->HasAnimations())
            {
                shader->SetMatrix("ubone_transforms", model->GetBoneTransforms()[0], static_cast<u32>(model->GetBoneTransforms().size()));
            }
            for (auto &mesh : model->GetMeshes())
            {
                MaterialManager::UpdateMaterial(mesh->material_index, mesh->material.buffer_data);

                if (mesh->material.transparent)
                {
                    glBindTextureUnit(3, m_SkyboxTexture); // bind texture index to renderID
                    shader->SetInt("urefract_map_cube_texture", 3);
                }
                
                shader->SetMatrix("umodel_transform", transform_comp.GetTransform() *mesh->transform);
                shader->SetInt("umaterial_index", mesh->material_index);
                mesh->material.Update(shader);
                RenderCommand::DrawIndexed(mesh->vertex_array);
            }

            shader->Disable();
        }
    }

    Renderer2D::End();

    for (const auto &[e, tc, env] : m_Registry.view<TransformComponent, EnvironmentMapComponent>().each())
    {
        Entity entity {e, this};
        EnvironmentMapComponent &env_map = m_Registry.get<EnvironmentMapComponent>(e);
        if (env_map.skybox)
        {
            glDepthFunc(GL_LEQUAL);
            Renderer::GetShader("Skybox")->Enable();
            Renderer::skybox_uniform_buffer->Bind();
            Renderer::skybox_uniform_buffer->SetData(&env_map.tint_color, sizeof(glm::vec4) + sizeof(f32));
            Renderer::skybox_uniform_buffer->SetData(&env_map.blur_factor, sizeof(glm::vec4) + sizeof(f32), sizeof(glm::vec4));
            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_CUBE_MAP, env_map.skybox->texture_id);

            m_SkyboxTexture = env_map.skybox->texture_id;
            Renderer::GetShader("Skybox")->SetInt("uskybox_cube", 0);
            RenderCommand::DrawIndexed(env_map.skybox->vao);
            glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
            Renderer::GetShader("Skybox")->Disable();
            Renderer::skybox_uniform_buffer->Unbind();
            glDepthFunc(GL_LESS);
        }
    }
}

void Scene::RenderStencilScene(const Camera &camera, entt::entity selectedId)
{
    glEnable(GL_STENCIL_TEST);
    glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);

#pragma region FIRST_PASS

    if (m_Registry.valid(selectedId))
    {
        Entity entity = { selectedId, this };
        TransformComponent &tc = entity.GetComponent<TransformComponent>();

        if (!tc.Visible)
        {
            glDisable(GL_STENCIL_TEST);
            return;
        }

        glStencilFunc(GL_ALWAYS, 1, 0xFF);
        glStencilMask(0xFF);

        // 2D Objects
        Renderer2D::Begin();
        if (entity.HasComponent<SpriteRenderer2DComponent>())
        {
            SpriteRenderer2DComponent &src = entity.GetComponent<SpriteRenderer2DComponent>();
            if (entity.HasComponent<SpriteAnimationComponent>())
            {
                SpriteAnimationComponent &ac = entity.GetComponent<SpriteAnimationComponent>();
                if (ac.State->IsCurrentAnimationExists())
                {
                    if (ac.State->GetAnimation()->HasFrame())
                    {
                        Ref<SpriteAnimation> &anim = ac.State->GetAnimation();
                        src.Texture = anim->GetCurrentFrame().Handle;
                        src.UV0 = anim->GetCurrentFrame().UV0;
                        src.UV1 = anim->GetCurrentFrame().UV1;
                    }
                }
            }

            Renderer2D::DrawSprite(tc.GetTransform(), src);
        }

        // Text
        if (entity.HasComponent<TextComponent>())
        {
            auto &text = entity.GetComponent<TextComponent>();
            glm::mat4 transform = glm::mat4(1.0f);
            glm::mat4 invertedCamTransform = glm::mat4(1.0f);

            glm::vec3 centerOffset = glm::vec3(text.Size.x / 2.0f, -text.Size.y / 2.0f + 1.0f, 0.0f);
            glm::vec3 scaledOffset = tc.WorldScale * centerOffset;
            glm::vec3 rotatedOffset = glm::toMat3(tc.WorldRotation) * scaledOffset;

            text.Position = tc.WorldTranslation - rotatedOffset;

            transform = glm::translate(glm::mat4(1.0f), text.Position)
                * glm::toMat4(tc.WorldRotation)
                * glm::scale(glm::mat4(1.0f), tc.WorldScale);

            if (text.FontHandle != 0)
            {
                Renderer2D::DrawString(text.TextString, transform, text);
            }
        }

        Renderer2D::End();

        // 3D Objects
#pragma endregion

#pragma region SECOND_PASS
            // Second pass: Draw the outline for the selected object
        glStencilFunc(GL_NOTEQUAL, 1, 0xFF);
        glStencilMask(0x00);

        glEnable(GL_DEPTH_TEST);  // Enable depth testing
        glDepthFunc(GL_LEQUAL);   // Change depth function to allow drawing on top of the object

        glm::vec3 cameraPosition = camera.GetPosition();
        glm::vec3 objectPosition = tc.WorldTranslation;
        float distance = glm::distance(cameraPosition, objectPosition);
        if (!camera.IsPerspective())
        {
            distance = camera.GetOrthoScale();
        }

        float baseThickness = 0.01f; // Adjust this value to change the base thickness
        float thicknessFactor = baseThickness * distance;

        thicknessFactor = glm::clamp(thicknessFactor, 0.01f, 2.0f);

        glm::mat4 scaledTransform = glm::translate(glm::mat4(1.0f), tc.WorldTranslation)
            * glm::toMat4(tc.WorldRotation)
            * glm::scale(glm::mat4(1.0f), tc.WorldScale + thicknessFactor);

        Shader *outlineShader = Renderer::GetShader("Outline").get();
        outlineShader->Enable();
        outlineShader->SetMatrix("viewProjection", camera.GetViewProjection());

        // 2D Objects
        Renderer2D::Begin(outlineShader);
        if (entity.HasComponent<SpriteRenderer2DComponent>())
        {
            SpriteRenderer2DComponent &src = entity.GetComponent<SpriteRenderer2DComponent>();
            if (entity.HasComponent<SpriteAnimationComponent>())
            {
                SpriteAnimationComponent &ac = entity.GetComponent<SpriteAnimationComponent>();
                if (ac.State->IsCurrentAnimationExists())
                {
                    if (ac.State->GetAnimation()->HasFrame())
                    {
                        Ref<SpriteAnimation> &anim = ac.State->GetAnimation();
                        src.Texture = anim->GetCurrentFrame().Handle;
                        src.UV0 = anim->GetCurrentFrame().UV0;
                        src.UV1 = anim->GetCurrentFrame().UV1;
                    }
                }
            }
            Renderer2D::DrawSprite(scaledTransform, src);
        }

        // Text
        if (entity.HasComponent<TextComponent>())
        {
            auto &text = entity.GetComponent<TextComponent>();
            glm::mat4 transform = glm::mat4(1.0f);
            glm::mat4 inverted_cam_transform = glm::mat4(1.0f);

            glm::vec3 centerOffset = glm::vec3(text.Size.x / 2.0f, -text.Size.y / 2.0f + 1.0f, 0.0f);
            glm::vec3 scaledOffset = tc.WorldScale * centerOffset;
            glm::vec3 rotatedOffset = glm::toMat3(tc.WorldRotation) * scaledOffset;

            text.Position = tc.WorldTranslation - rotatedOffset;

            transform = glm::translate(glm::mat4(1.0f), text.Position)
                * glm::toMat4(tc.WorldRotation)
                * glm::scale(glm::mat4(1.0f), tc.WorldScale);

            if (text.FontHandle != 0)
            {
                Renderer2D::DrawString(text.TextString, transform, text);
            }
        }

        Renderer2D::End();
#pragma endregion
    }

    glDisable(GL_STENCIL_TEST);
}

void Scene::DestroyEntityRecursive(UUID entityId)
{
    if (Entity entity = GetEntityWithUUID(entityId))
    {
        for (std::vector<UUID> childrenIds = GetChildrenUUIDs(entityId); const auto &childId : childrenIds)
        {
            entity.GetComponent<IDComponent>().RemoveChild(childId);
            DestroyEntityRecursive(childId);
        }

        m_Registry.destroy(static_cast<entt::entity>(entity));
        m_EntityStorage.erase(std::ranges::remove_if(m_EntityStorage,
                                                     [entityId](const auto &pair) { return pair.first == entityId; }).begin(),
            m_EntityStorage.end());
    }
}

void Scene::DestroyEntity(Entity entity)
{
    const UUID uuid = entity.GetUUID();
    DestroyEntityRecursive(uuid);
}

Entity Scene::DuplicateEntityRecursive(Entity entity, Entity newParent)
{
    std::string name = entity.GetTag();
    Entity newEntity = EntityManager::CreateEntity(name, this, entity.GetType());
    EntityManager::CopyComponentIfExists(AllComponents{}, newEntity, entity);

    auto &newEntityIDC = newEntity.GetComponent<IDComponent>();
    if (newParent)
    {
        newEntityIDC.Parent = newParent.GetUUID();
        newParent.GetComponent<IDComponent>().AddChild(newEntity.GetUUID());
    }
    else if (entity.HasParent())
    {
        Entity parent = GetEntityWithUUID(entity.GetParentUUID());
        newEntityIDC.Parent = parent.GetUUID();
        parent.GetComponent<IDComponent>().AddChild(newEntity.GetUUID());

    }
    else
    {
        newEntityIDC.Parent = UUID(0);
    }

    auto view = m_Registry.view<IDComponent>();
    for (auto e : view)
    {
        Entity child = { e, this };
        if (child && child.GetComponent<IDComponent>().Parent == entity.GetUUID())
        {
            DuplicateEntityRecursive(child, newEntity);
        }
    }
    return newEntity;
}

Entity Scene::DuplicateEntity(Entity entity)
{
    return DuplicateEntityRecursive(entity, {});
}

std::vector<UUID> Scene::GetChildrenUUIDs(UUID parentId)
{
    std::vector<UUID> children_uui_ds;
    m_Registry.view<IDComponent>().each([&](auto entity, const IDComponent idc)
    {
        if (idc.Parent == parentId)
        {
            children_uui_ds.push_back(idc.ID);
        }
    });
    return children_uui_ds;
}

Entity Scene::GetPrimaryCameraEntity()
{
    OGN_PROFILER_SCENE();

    if (m_EntityStorage.size())
    {
        auto view = m_Registry.view<CameraComponent>();
        for (auto &entity : view)
        {
            const CameraComponent &camera = view.get<CameraComponent>(entity);
            if (camera.Primary)
            {
                return { entity, this };
            }
        }
    }

    return { entt::null, nullptr };
}

void Scene::SetFocus(bool focus)
{
    m_IsFocus = focus;
}

void Scene::LockMouse()
{
    m_IsFocus = true;
    Input::SetCursoreMode(CursorMode::Lock);
}

void Scene::UnlockMouse()
{
    m_IsFocus = false;
    Input::SetCursoreMode(CursorMode::Default);
}

void Scene::OnViewportResize(const u32 width, const u32 height)
{
    OGN_PROFILER_SCENE();

    const auto &view = m_Registry.view<CameraComponent>();

    for (auto &e : view)
    {
        auto &cc = view.get<CameraComponent>(e);
        if (cc.Primary)
        {
            cc.Camera.SetViewportSize(width, height);
            const glm::ivec2 &vp = cc.Camera.GetViewportSize();
            const glm::vec2 &ortho = cc.Camera.GetOrthoSize();
            m_UIRenderer->SetViewportSize(vp.x, vp.y, ortho.x, ortho.y);
        }
    }

    m_ViewportWidth = width;
    m_ViewportHeight = height;
}

void Scene::Step(const i32 frames)
{
    m_StepFrames = frames;
}

template <typename T>
void Scene::OnComponentAdded(Entity entity, T &component)
{
}

#define OGN_ADD_COMPONENT(ComponentType)\
	template<>\
	void Scene::OnComponentAdded<ComponentType>(Entity entity, ComponentType &c){}

OGN_ADD_COMPONENT(IDComponent);
OGN_ADD_COMPONENT(TagComponent);
OGN_ADD_COMPONENT(TransformComponent);
OGN_ADD_COMPONENT(UIComponent);
OGN_ADD_COMPONENT(AudioListenerComponent);
OGN_ADD_COMPONENT(AudioComponent);
OGN_ADD_COMPONENT(SpotLightComponent);
OGN_ADD_COMPONENT(SpriteRenderer2DComponent);
OGN_ADD_COMPONENT(SpriteAnimationComponent);
OGN_ADD_COMPONENT(MeshComponent);
OGN_ADD_COMPONENT(TextComponent);
OGN_ADD_COMPONENT(CircleRendererComponent);
OGN_ADD_COMPONENT(NativeScriptComponent);
OGN_ADD_COMPONENT(ScriptComponent);
OGN_ADD_COMPONENT(ParticleComponent);
OGN_ADD_COMPONENT(Rigidbody2DComponent);
OGN_ADD_COMPONENT(BoxCollider2DComponent);
OGN_ADD_COMPONENT(CircleCollider2DComponent);
OGN_ADD_COMPONENT(RevoluteJoint2DComponent);
OGN_ADD_COMPONENT(RigidbodyComponent);

template<>
void Scene::OnComponentAdded<CameraComponent>(Entity entity, CameraComponent &component)
{
    if (m_ViewportWidth > 0 && m_ViewportHeight > 0)
    {
        component.Camera.SetViewportSize(m_ViewportWidth, m_ViewportHeight);
    }
}

template<>
void Scene::OnComponentAdded(Entity entity, BoxColliderComponent &component)
{
    if (!entity.HasComponent<RigidbodyComponent>()) entity.AddComponent<RigidbodyComponent>();
}

template<>
void Scene::OnComponentAdded(Entity entity, SphereColliderComponent &component)
{
    if (!entity.HasComponent<RigidbodyComponent>()) entity.AddComponent<RigidbodyComponent>();
}

template<>
void Scene::OnComponentAdded(Entity entity, CapsuleColliderComponent &component)
{
    if (!entity.HasComponent<RigidbodyComponent>()) entity.AddComponent<RigidbodyComponent>();
}

template<>
void Scene::OnComponentAdded(Entity entity, DirectionalLightComponent &component)
{
    component.Light = Lighting::Create<DirectionalLight>();
}

template<>
void Scene::OnComponentAdded(Entity entity, PointLightComponent &component)
{
    component.Light = Lighting::Create<PointLight>();

    Ref<PointLight> pl = std::static_pointer_cast<PointLight>(component.Light);
    LightingManager::GetInstance()->AddPointLight(pl->data);
}

template<>
void Scene::OnComponentAdded(Entity entity, EnvironmentMapComponent &component)
{
    component.skybox = Skybox::Create("Resources/Skybox", ".jpg");
}

}
