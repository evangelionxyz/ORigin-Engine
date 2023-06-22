// Copyright (c) 2022 Evangelion Manuhutu | ORigin Engine

#include "pch.h"
#include "Entity.h"
#include "Scene.h"
#include "Origin\Audio\Audio.h"
#include "ScriptableEntity.h"
#include "Component.h"

#include "Origin/Scripting/ScriptEngine.h"

#include "Origin/Renderer/Renderer.h"
#include "Origin/Renderer/Renderer2D.h"
#include "Origin/Renderer/Renderer3D.h"

// Box2D
#include "box2d/b2_world.h"
#include "box2d/b2_body.h"
#include "box2d/b2_fixture.h"
#include "box2d/b2_polygon_shape.h"
#include "box2d/b2_circle_shape.h"

#include <glm/glm.hpp>

namespace origin
{
    static b2BodyType Box2DBodyType(Rigidbody2DComponent::BodyType type)
    {
        switch (type)
        {
        case origin::Rigidbody2DComponent::BodyType::Static: return b2BodyType::b2_staticBody;
        case origin::Rigidbody2DComponent::BodyType::Dynamic: return b2BodyType::b2_dynamicBody;
        case origin::Rigidbody2DComponent::BodyType::Kinematic: return b2BodyType::b2_kinematicBody;
        }

        OGN_ASSERT(false, "Unkown Body Type");
        return b2_staticBody;
    }

    Scene::Scene()
    {
        m_CameraIcon = Texture2D::Create("Resources/UITextures/camera.png");
        m_LightingIcon = Texture2D::Create("Resources/UITextures/lighting.png");
        m_AudioIcon = Texture2D::Create("Resources/UITextures/audio.png");
    }

    Scene::~Scene()
    {
    }

    template <typename... Component>
    static void CopyComponent(entt::registry& dst, entt::registry& src,
                              const std::unordered_map<UUID, entt::entity>& enttMap)
    {
        ([&]()
        {
            auto view = src.view<Component>();
            for (auto srcEntity : view)
            {
                entt::entity dstEntity = enttMap.at(src.get<IDComponent>(srcEntity).ID);

                auto& srcComponent = src.get<Component>(srcEntity);
                dst.emplace_or_replace<Component>(dstEntity, srcComponent);
            }
        }(), ...);
    }

    template <typename... Component>
    static void CopyComponent(ComponentGroup<Component...>, entt::registry& dst, entt::registry& src,
                              const std::unordered_map<UUID, entt::entity>& enttMap)
    {
        CopyComponent<Component...>(dst, src, enttMap);
    }

    template <typename... Component>
    static void CopyComponentIfExists(Entity dst, Entity src)
    {
        ([&]()
        {
            if (src.HasComponent<Component>())
                dst.AddOrReplaceComponent<Component>(src.GetComponent<Component>());
        }(), ...);
    }

    template <typename... Component>
    static void CopyComponentIfExists(ComponentGroup<Component...>, Entity dst, Entity src)
    {
        CopyComponentIfExists<Component...>(dst, src);
    }

    std::shared_ptr<Scene> Scene::Copy(std::shared_ptr<Scene> other)
    {
        auto newScene = std::make_shared<Scene>();

        newScene->m_ViewportWidth = other->m_ViewportWidth;
        newScene->m_ViewportHeight = other->m_ViewportHeight;

        auto& srcSceneRegistry = other->m_Registry;
        auto& dstSceneRegistry = newScene->m_Registry;
        std::unordered_map<UUID, entt::entity> enttMap;
        auto newEntity = Entity();

        // create entities in new scene
        auto& idView = srcSceneRegistry.view<IDComponent>();
        for (auto e : idView)
        {
            UUID uuid = srcSceneRegistry.get<IDComponent>(e).ID;

            const auto& name = srcSceneRegistry.get<TagComponent>(e).Tag;
            newEntity = newScene->CreateEntityWithUUID(uuid, name);

            enttMap[uuid] = (entt::entity)newEntity;
        }

        // Copy components (except IDComponent and TagComponent) into new scene (destination)
        CopyComponent(AllComponents{}, dstSceneRegistry, srcSceneRegistry, enttMap);

        return newScene;
    }

    Entity Scene::CreateEntity(const std::string& name)
    {
        return CreateEntityWithUUID(UUID(), name);
    }

    Entity Scene::CreateEntityWithUUID(UUID uuid, const std::string& name)
    {
        Entity entity = {m_Registry.create(), this};
        entity.AddComponent<IDComponent>(uuid);
        entity.AddComponent<TransformComponent>();
        auto& tag = entity.AddComponent<TagComponent>();
        tag.Tag = name.empty() ? "Entity" : name;

        m_EntityMap.insert(std::make_pair(uuid, entity));

        return entity;
    }

    Entity Scene::CreatePointlight(const std::string& name)
    {
        Entity entity = {m_Registry.create(), this};
        entity.AddComponent<IDComponent>(UUID());
        entity.AddComponent<TransformComponent>();
        entity.AddComponent<PointLightComponent>();

        auto& tag = entity.AddComponent<TagComponent>();
        tag.Tag = name.empty() ? "Entity" : name;

        UUID& uuid = entity.GetComponent<IDComponent>().ID;

        m_EntityMap.insert(std::make_pair(uuid, entity));

        return entity;
    }

    Entity Scene::CreateSpotLight(const std::string& name)
    {
        Entity entity = {m_Registry.create(), this};
        entity.AddComponent<IDComponent>(UUID());
        entity.AddComponent<TransformComponent>();
        entity.AddComponent<SpotLightComponent>();

        auto& tag = entity.AddComponent<TagComponent>();
        tag.Tag = name.empty() ? "Entity" : name;

        UUID& uuid = entity.GetComponent<IDComponent>().ID;

        m_EntityMap.insert(std::make_pair(uuid, entity));

        return entity;
    }

    Entity Scene::CreateMesh(const std::string& name)
    {
        Entity entity = {m_Registry.create(), this};
        entity.AddComponent<IDComponent>(UUID());
        entity.AddComponent<TransformComponent>();
        entity.AddComponent<StaticMeshComponent>();

        auto& tag = entity.AddComponent<TagComponent>();
        tag.Tag = name.empty() ? "Entity" : name;

        UUID& uuid = entity.GetComponent<IDComponent>().ID;
        m_EntityMap.insert(std::make_pair(uuid, entity));

        return entity;
    }

    Entity Scene::CreateSpriteEntity(const std::string& name)
    {
        Entity entity = {m_Registry.create(), this};
        entity.AddComponent<IDComponent>(UUID());
        entity.AddComponent<TransformComponent>();
        entity.AddComponent<SpriteRenderer2DComponent>();

        auto& tag = entity.AddComponent<TagComponent>();
        tag.Tag = name.empty() ? "Entity" : name;

        UUID& uuid = entity.GetComponent<IDComponent>().ID;
        m_EntityMap.insert(std::make_pair(uuid, entity));

        return entity;
    }

    Entity Scene::CreateCube(const std::string& name)
    {
        Entity entity = {m_Registry.create(), this};
        entity.AddComponent<IDComponent>(UUID());
        entity.AddComponent<TransformComponent>();
        entity.AddComponent<SpriteRendererComponent>();

        auto& tag = entity.AddComponent<TagComponent>();
        tag.Tag = name.empty() ? "Entity" : name;

        UUID& uuid = entity.GetComponent<IDComponent>().ID;
        m_EntityMap.insert(std::make_pair(uuid, entity));

        return entity;
    }

    Entity Scene::CreateCamera(const std::string& name)
    {
        Entity entity = {m_Registry.create(), this};

        entity.AddComponent<IDComponent>();
        entity.AddComponent<TransformComponent>();
        entity.AddComponent<CameraComponent>();

        auto& tag = entity.AddComponent<TagComponent>();
        tag.Tag = name.empty() ? "Camera" : name;

        auto& translation = entity.GetComponent<TransformComponent>().Translation;
        translation.z = 8.0f;

        UUID& uuid = entity.GetComponent<IDComponent>().ID;
        m_EntityMap.insert(std::make_pair(uuid, entity));

        return entity;
    }

    Entity Scene::CreateCircle(const std::string& name)
    {
        Entity entity = {m_Registry.create(), this};

        entity.AddComponent<IDComponent>();
        entity.AddComponent<TransformComponent>();
        entity.AddComponent<CircleRendererComponent>();

        auto& tag = entity.AddComponent<TagComponent>();
        tag.Tag = name.empty() ? "Circle" : name;

        UUID& uuid = entity.GetComponent<IDComponent>().ID;
        m_EntityMap.insert(std::make_pair(uuid, entity));

        return entity;
    }

    void Scene::DestroyEntity(Entity entity)
    {
        m_EntityMap.erase(entity.GetUUID());
        m_Registry.destroy(entity);
    }

    void Scene::OnUpdateRuntime(Timestep time)
    {
      if (!m_Paused || m_StepFrames-- > 0)
      {
        // Update Scripts
        {
          auto& view = m_Registry.view<ScriptComponent>();
          for (auto& e : view)
          {
            Entity entity = { e, this };
            ScriptEngine::OnUpdateEntity(entity, time);
          }

          m_Registry.view<NativeScriptComponent>().each([=](auto entity, auto& nsc)
          {
            if (!nsc.Instance)
            {
              nsc.Instance = nsc.InstantiateScript();
              nsc.Instance->m_Entity = Entity{ entity, this };
              nsc.Instance->OnCreate();
            }
            nsc.Instance->OnUpdate(time);
          });
        }

        // Physics
        {
          const int32_t velocityIterations = 6;
          const int32_t positionIterations = 2;
          m_Box2DWorld->Step(time, velocityIterations, positionIterations);

          // Retrieve transform from Box2D
          auto& view = m_Registry.view<Rigidbody2DComponent>();
          for (auto& e : view)
          {
            Entity entity = { e, this };
            auto& transform = entity.GetComponent<TransformComponent>();
            auto& rb2d = entity.GetComponent<Rigidbody2DComponent>();

            auto body = (b2Body*)rb2d.RuntimeBody;

            const auto& position = body->GetPosition();

            transform.Translation.x = position.x;
            transform.Translation.y = position.y;
            transform.Rotation.z = body->GetAngle();
          }
        }
      }

      // Rendering
      Camera* mainCamera = nullptr;
      TransformComponent cameraTransform;
      auto& view = m_Registry.view<CameraComponent, TransformComponent>();
      for (auto entity : view)
      {
          auto& [transform, camera] = view.get<TransformComponent, CameraComponent>(entity);

          if (camera.Primary)
          {
              mainCamera = &camera.Camera;
              cameraTransform = transform;
              break;
          }
      }

      if (mainCamera)
          RenderScene(mainCamera, cameraTransform);

      // Audio Update
      {
        auto view = m_Registry.view<TransformComponent, AudioComponent>();
        for (auto entity : view)
        {
          auto& [tc, ac] = view.get<TransformComponent, AudioComponent>(entity);
          if (ac.Audio)
          {
            ac.Audio->SetMaxDistance(ac.MaxDistance);
            ac.Audio->SetMinDistance(ac.MinDistance);
            ac.Audio->SetLoop(ac.Looping);
            ac.Audio->SetGain(ac.Volume);

            if (ac.Spatial)
            {
              ac.Audio->SetPosition(tc.Translation);
            }
          }
        }

        AudioEngine::SetListener(cameraTransform.Translation, cameraTransform.GetForward(), cameraTransform.GetUp());
        AudioEngine::SystemUpdate();
      }
    }

    void Scene::OnUpdateEditor(Timestep time, EditorCamera& camera)
    {
      m_Registry.view<NativeScriptComponent>().each([=](auto entity, auto& nsc)
      {
          if (!nsc.Instance)
          {
              nsc.Instance = nsc.InstantiateScript();
              nsc.Instance->m_Entity = Entity{entity, this};
              nsc.Instance->OnCreate();
          }
          nsc.Instance->OnUpdate(time);
      });

      //Render
      RenderScene(camera);

      // Audio Update
      {
        Renderer2D::BeginScene();
        auto view = m_Registry.view<TransformComponent, AudioComponent>();
        for (auto entity : view)
        {
          auto& [tc, ac] = view.get<TransformComponent, AudioComponent>(entity);
          if (ac.Audio)
          {
            ac.Audio->SetMaxDistance(ac.MaxDistance);
            ac.Audio->SetMinDistance(ac.MinDistance);
            ac.Audio->SetLoop(ac.Looping);
            ac.Audio->SetGain(ac.Volume);

            if (ac.Spatial)
            {
              ac.Audio->SetPosition(tc.Translation);
            }
          }
          DrawIcon(camera, (int)entity, m_AudioIcon, tc, true);
        }
        Renderer2D::EndScene();

        AudioEngine::SetListener(camera.GetPosition(), camera.GetForwardDirection(), glm::vec3(0.0f, 1.0f, 0.0f));
        AudioEngine::SystemUpdate();
      }
    }

    void Scene::OnUpdateSimulation(Timestep time, EditorCamera& camera)
    {
      if (!m_Paused || m_StepFrames-- > 0)
      {
        // Update Scripts
        {
          auto& view = m_Registry.view<ScriptComponent>();
          for (auto e : view)
          {
              Entity entity = {e, this};
              ScriptEngine::OnUpdateEntity(entity, time);
          }

          m_Registry.view<NativeScriptComponent>().each([=](auto entity, auto& nsc)
          {
              if (!nsc.Instance)
              {
                  nsc.Instance = nsc.InstantiateScript();
                  nsc.Instance->m_Entity = Entity{entity, this};
                  nsc.Instance->OnCreate();
              }
              nsc.Instance->OnUpdate(time);
          });
        }

        // Physics
        {
          const int32_t velocityIterations = 6;
          const int32_t positionIterations = 2;
          m_Box2DWorld->Step(time, velocityIterations, positionIterations);

          // Retrieve transform from Box2D
          auto view = m_Registry.view<Rigidbody2DComponent>();
          for (auto e : view)
          {
              Entity entity = {e, this};
              auto& transform = entity.GetComponent<TransformComponent>();
              auto& rb2d = entity.GetComponent<Rigidbody2DComponent>();

              auto body = (b2Body*)rb2d.RuntimeBody;

              const auto& position = body->GetPosition();

              transform.Translation.x = position.x;
              transform.Translation.y = position.y;
              transform.Rotation.z = body->GetAngle();
          }
        }
      }

      // Render
      RenderScene(camera);

      // Audio Update
      {
        Renderer2D::BeginScene();
        auto view = m_Registry.view<TransformComponent, AudioComponent>();
        for (auto entity : view)
        {
          auto& [tc, ac] = view.get<TransformComponent, AudioComponent>(entity);
          if (ac.Audio)
          {
            ac.Audio->SetMaxDistance(ac.MaxDistance);
            ac.Audio->SetMinDistance(ac.MinDistance);
            ac.Audio->SetLoop(ac.Looping);
            ac.Audio->SetGain(ac.Volume);

            if (ac.Spatial)
            {
              ac.Audio->SetPosition(tc.Translation);
            }
          }
          DrawIcon(camera, (int)entity, m_AudioIcon, tc, true);
        }
        Renderer2D::EndScene();
      }

      AudioEngine::SetListener(camera.GetPosition(), camera.GetForwardDirection(), glm::vec3(0.0f, 1.0f, 0.0f));
      AudioEngine::SystemUpdate();
    }

    void Scene::OnSimulationStart()
    {
      {
        OnPhysics2DStart();

        // Scripting
        ScriptEngine::OnRuntimeStart(this);
        auto view = m_Registry.view<ScriptComponent>();
        for (auto e : view)
        {
          Entity entity = { e, this };
          ScriptEngine::OnCreateEntity(entity);
        }

        // Audio
        {
          auto view = m_Registry.view<AudioComponent>();
          for (auto& e : view)
          {
            auto& ac = view.get<AudioComponent>(e);
            if (ac.Audio && ac.PlayAtStart)
            {
              ac.Audio->SetGain(ac.Volume);
              ac.Audio->Play();
            }
          }
        }
      }
    }

    void Scene::OnSimulationStop()
    {
        OnPhysics2DStop();
        ScriptEngine::OnRuntimeStop();

        // Audio
        {
          auto view = m_Registry.view<AudioComponent>();
          for (auto& e : view)
          {
            auto& ac = view.get<AudioComponent>(e);
            if (ac.Audio)
              ac.Audio->Stop();
          }
        }
    }

    void Scene::RenderScene(Camera* camera, const TransformComponent& cameraTransform)
    {
        Renderer::BeginScene(*camera, cameraTransform.GetTransform());
        // Get Camera Position
        const glm::vec3& MainCameraPosition = cameraTransform.Translation;

        // Sprites
        {
            auto& view = m_Registry.view<TransformComponent, SpriteRenderer2DComponent>();
            std::vector<entt::entity> spriteEntities(view.begin(), view.end());

            std::sort(spriteEntities.begin(), spriteEntities.end(),
                      [=](const entt::entity& a, const entt::entity& b)
                      {
                          const auto& objA = m_Registry.get<TransformComponent>(a);
                          const auto& objB = m_Registry.get<TransformComponent>(b);
                          return glm::length(MainCameraPosition.z - objA.Translation.z) > glm::length(
                              MainCameraPosition.z - objB.Translation.z);
                      });

            for (const entt::entity& entity : spriteEntities)
            {
                auto& [transform, sprite] = view.get<TransformComponent, SpriteRenderer2DComponent>(entity);
                Renderer2D::DrawSprite(transform.GetTransform(), sprite, (int)entity);
            }

            Renderer::EndScene();
        }

        // Circles
        {
            auto& view = m_Registry.view<TransformComponent, CircleRendererComponent>();
            for (auto& entity : view)
            {
                auto [transform, circle] = view.get<TransformComponent, CircleRendererComponent>(entity);
                Renderer2D::DrawCircle(transform.GetTransform(), circle.Color, circle.Thickness, circle.Fade,
                                       (int)entity);
            }
        }


        // Text
        {
          auto& view = m_Registry.view<TransformComponent, TextComponent>();
          for (auto entity : view)
          {
            auto [transform, text] = view.get<TransformComponent, TextComponent>(entity);
            Renderer2D::DrawString(text.TextString, transform.GetTransform(), text, (int)entity);
          }
        }

        // 3D Scene
        auto& view = m_Registry.view<TransformComponent, SpriteRendererComponent>();
        for (auto entity : view)
        {
            auto& [transform, sprite] = view.get<TransformComponent, SpriteRendererComponent>(entity);
            Renderer3D::DrawCube(transform.GetTransform(), sprite, static_cast<int>(entity));
        }

        {
            // Mesh
            auto& view = m_Registry.view<TransformComponent, StaticMeshComponent>();
            for (auto& entity : view)
            {
                auto& [tc, sMesh] = view.get<TransformComponent, StaticMeshComponent>(entity);
                // ================
                // Point Light Data
                // ================
                int pointLight = 0;
                auto pointLightView = m_Registry.view<TransformComponent, PointLightComponent>();
                for (auto entity : pointLightView)
                {
                    auto& [transform, light] = pointLightView.get<TransformComponent, PointLightComponent>(entity);
                    pointLight++;
                    if (sMesh.Model)
                    {
                        std::string uniformName = "pointLights[" + std::to_string(pointLight) + "].";
                        sMesh.Model->GetShader()->Bind();
                        sMesh.Model->GetShader()->SetInt("pointLightsCount", pointLight);
                        sMesh.Model->GetShader()->SetVector(uniformName + "Position", transform.Translation);
                        sMesh.Model->GetShader()->SetVector(uniformName + "Color", light.Color);
                        sMesh.Model->GetShader()->SetFloat(uniformName + "Ambient", light.Ambient);
                        sMesh.Model->GetShader()->SetFloat(uniformName + "Specular", light.Specular);
                    }
                }

                // ================
                // Spot Light Data
                // ================
                int spotLight = 0;
                auto& spotLightView = m_Registry.view<TransformComponent, SpotLightComponent>();
                for (auto entity : spotLightView)
                {
                    auto& [transform, light] = spotLightView.get<TransformComponent, SpotLightComponent>(entity);
                    if (sMesh.Model)
                    {
                        //std::string uniformName = "spotLights[" + std::to_string(spotLight) + "].";
                        std::string uniformName = "spotLight.";
                        sMesh.Model->GetShader()->Bind();
                        sMesh.Model->GetShader()->SetVector(uniformName + "Position", transform.Translation);
                        sMesh.Model->GetShader()->SetVector(uniformName + "Color", light.Color);
                        sMesh.Model->GetShader()->SetFloat(uniformName + "Ambient", light.Ambient);
                        sMesh.Model->GetShader()->SetFloat(uniformName + "Specular", light.Specular);
                        sMesh.Model->GetShader()->SetFloat(uniformName + "Innercone", light.Innercone);
                        sMesh.Model->GetShader()->SetFloat(uniformName + "Outercone", light.Outercone);
                        spotLight++;
                    }
                }
                // ================
                // Directional Light Data
                // ================
                auto& directionalLightView = m_Registry.view<TransformComponent, DirectionalLightComponent>();
                for (auto entity : directionalLightView)
                {
                    auto& [transform, light] = directionalLightView.get<TransformComponent, DirectionalLightComponent>(entity);
                    if (sMesh.Model)
                    {
                        std::string uniformName = "directionalLight.";
                        sMesh.Model->GetShader()->Bind();
                        sMesh.Model->GetShader()->SetVector(uniformName + "Direction",
                                                            glm::normalize(glm::vec3(0.0f, -1.0f, 0.0f)));
                        sMesh.Model->GetShader()->SetFloat(uniformName + "Ambient", light.Ambient);
                        sMesh.Model->GetShader()->SetFloat(uniformName + "Diffuse", light.Diffuse);
                        sMesh.Model->GetShader()->SetFloat(uniformName + "Specular", light.Specular);
                    }
                }

                if (sMesh.Model)
                {
                    sMesh.Model->GetShader()->Bind();
                    sMesh.Model->GetShader()->SetVector("uColor", sMesh.Color);
                    sMesh.Model->GetShader()->SetVector("uCameraPosition", MainCameraPosition);

                    sMesh.Model->Draw(tc.GetTransform(), camera, static_cast<int>(entity));
                }
            }
        }

        Renderer::EndScene();
    }

    void Scene::RenderScene(const EditorCamera& camera)
    {
        Renderer::BeginScene(camera);

        // Sprites
        {
            auto& view = m_Registry.view<TransformComponent, SpriteRenderer2DComponent>();
            std::vector<entt::entity> spriteEntities(view.begin(), view.end());

            std::sort(spriteEntities.begin(), spriteEntities.end(),
                      [=](const entt::entity& a, const entt::entity& b)
                      {
                          const auto& objA = m_Registry.get<TransformComponent>(a);
                          const auto& objB = m_Registry.get<TransformComponent>(b);
                          return glm::length(camera.GetPosition() - objA.Translation) > glm::length(
                              camera.GetPosition() - objB.Translation);
                      });

            for (const entt::entity& entity : spriteEntities)
            {
                auto& [transform, sprite] = view.get<TransformComponent, SpriteRenderer2DComponent>(entity);
                Renderer2D::DrawSprite(transform.GetTransform(), sprite, (int)entity);
            }
            Renderer::EndScene();
        }

        // Circles
        {
            auto& view = m_Registry.view<TransformComponent, CircleRendererComponent>();
            for (auto& entity : view)
            {
                auto& [transform, circle] = view.get<TransformComponent, CircleRendererComponent>(entity);
                Renderer2D::DrawCircle(transform.GetTransform(), circle.Color, circle.Thickness, circle.Fade,
                                       (int)entity);
            }
        }

        // Text
        {
          auto& view = m_Registry.view<TransformComponent, TextComponent>();
          for (auto entity : view)
          {
            auto [transform, text] = view.get<TransformComponent, TextComponent>(entity);
            Renderer2D::DrawString(text.TextString, transform.GetTransform(), text, (int)entity);
          }
        }

        // 3D Scene
        // Cube
        auto& cubeView = m_Registry.view<TransformComponent, SpriteRendererComponent>();
        std::vector<entt::entity> cubeEntities(cubeView.begin(), cubeView.end());
        std::sort(cubeEntities.begin(), cubeEntities.end(),
                  [=](const entt::entity& a, const entt::entity& b)
                  {
                      const auto& objA = m_Registry.get<TransformComponent>(a);
                      const auto& objB = m_Registry.get<TransformComponent>(b);
                      return glm::length(camera.GetPosition() - objA.Translation) > glm::length(
                          camera.GetPosition() - objB.Translation);
                  });

        for (const entt::entity cube : cubeEntities)
        {
            auto& [transform, sprite] = cubeView.get<TransformComponent, SpriteRendererComponent>(cube);
            Renderer3D::DrawCube(transform.GetTransform(), sprite, (int)cube);
        }

        // Camera
        {
            auto& view = m_Registry.view<TransformComponent, CameraComponent>();
            for (auto& entity : view)
            {
                auto& [tc, cc] = view.get<TransformComponent, CameraComponent>(entity);
                DrawIcon(camera, (int)entity, m_CameraIcon, tc, true);
            }
        }

        {
            // Mesh
            auto& view = m_Registry.view<TransformComponent, StaticMeshComponent>();
            for (auto& entity : view)
            {
                auto& [tc, sMesh] = view.get<TransformComponent, StaticMeshComponent>(entity);
                // ================
                // Point Light Data
                // ================
                int pointLight = 0;
                auto pointLightView = m_Registry.view<TransformComponent, PointLightComponent>();
                for (auto entity : pointLightView)
                {
                    auto& [transform, light] = pointLightView.get<TransformComponent, PointLightComponent>(entity);
                    DrawIcon(camera, (int)entity, m_LightingIcon, transform, true);
                    pointLight++;
                    if (sMesh.Model)
                    {
                        std::string uniformName = "pointLights[" + std::to_string(pointLight) + "].";
                        sMesh.Model->GetShader()->Bind();
                        sMesh.Model->GetShader()->SetInt("pointLightsCount", pointLight);
                        sMesh.Model->GetShader()->SetVector(uniformName + "Position", transform.Translation);
                        sMesh.Model->GetShader()->SetVector(uniformName + "Color", light.Color);
                        sMesh.Model->GetShader()->SetFloat(uniformName + "Ambient", light.Ambient);
                        sMesh.Model->GetShader()->SetFloat(uniformName + "Specular", light.Specular);
                    }
                }

                // ================
                // Spot Light Data
                // ================
                int spotLight = 0;
                auto& spotLightView = m_Registry.view<TransformComponent, SpotLightComponent>();
                for (auto entity : spotLightView)
                {
                    auto& [transform, light] = spotLightView.get<TransformComponent, SpotLightComponent>(entity);
                    DrawIcon(camera, (int)entity, m_LightingIcon, transform, true);
                    if (sMesh.Model)
                    {
                        //std::string uniformName = "spotLights[" + std::to_string(spotLight) + "].";
                        std::string uniformName = "spotLight.";
                        sMesh.Model->GetShader()->Bind();
                        sMesh.Model->GetShader()->SetVector(uniformName + "Position", transform.Translation);
                        sMesh.Model->GetShader()->SetVector(uniformName + "Color", light.Color);
                        sMesh.Model->GetShader()->SetFloat(uniformName + "Ambient", light.Ambient);
                        sMesh.Model->GetShader()->SetFloat(uniformName + "Specular", light.Specular);
                        sMesh.Model->GetShader()->SetFloat(uniformName + "Innercone", light.Innercone);
                        sMesh.Model->GetShader()->SetFloat(uniformName + "Outercone", light.Outercone);
                        spotLight++;
                    }
                }
                // ================
                // Directional Light Data
                // ================
                auto& directionalLightView = m_Registry.view<TransformComponent, DirectionalLightComponent>();
                for (auto entity : directionalLightView)
                {
                    auto& [transform, light] = directionalLightView.get<TransformComponent, DirectionalLightComponent>(
                        entity);
                    DrawIcon(camera, (int)entity, m_LightingIcon, transform, true);
                    if (sMesh.Model)
                    {
                        std::string uniformName = "directionalLight.";
                        sMesh.Model->GetShader()->Bind();
                        sMesh.Model->GetShader()->SetVector(uniformName + "Direction",
                                                            glm::normalize(glm::vec3(0.0f, -1.0f, 0.0f)));
                        sMesh.Model->GetShader()->SetFloat(uniformName + "Ambient", light.Ambient);
                        sMesh.Model->GetShader()->SetFloat(uniformName + "Diffuse", light.Diffuse);
                        sMesh.Model->GetShader()->SetFloat(uniformName + "Specular", light.Specular);
                    }
                }

                if (sMesh.Model)
                {
                    sMesh.Model->GetShader()->Bind();
                    sMesh.Model->GetShader()->SetVector("uCameraPosition", camera.GetPosition());
                    sMesh.Model->GetShader()->SetVector("uColor", sMesh.Color);

                    sMesh.Model->Draw(tc.GetTransform(), camera, (int)entity);
                }
            }
        }

        //DrawGrid(m_GridSize, m_GridColor);
        Renderer::EndScene();
    }

    void Scene::OnRuntimeStart()
    {
        OnPhysics2DStart();

        // Scripting
        {
            ScriptEngine::OnRuntimeStart(this);
            auto view = m_Registry.view<ScriptComponent>();
            for (auto e : view)
            {
                Entity entity = {e, this};
                ScriptEngine::OnCreateEntity(entity);
            }
        }

        // Audio
        {
          auto view = m_Registry.view<AudioComponent>();
          for(auto& e : view)
          {
            auto& ac = view.get<AudioComponent>(e);
            if (ac.Audio && ac.PlayAtStart)
            {
              ac.Audio->SetGain(ac.Volume);
              ac.Audio->Play();
            }
          }
        }
    }

    void Scene::OnRuntimeStop()
    {
        OnPhysics2DStop();
        ScriptEngine::OnRuntimeStop();

        // Audio
        {
          auto view = m_Registry.view<AudioComponent>();
          for (auto& e : view)
          {
            auto& ac = view.get<AudioComponent>(e);
            if (ac.Audio)
              ac.Audio->Stop();
          }
        }
    }

    void Scene::OnViewportResize(const uint32_t width, const uint32_t height)
    {
        if (m_ViewportHeight == height && m_ViewportWidth == width)
            return;

        m_ViewportWidth = width;
        m_ViewportHeight = height;

        const auto view = m_Registry.view<CameraComponent>();
        for (auto& entity : view)
        {
            auto& cameraComponent = view.get<CameraComponent>(entity);
            if (!cameraComponent.FixedAspectRatio)
                cameraComponent.Camera.SetViewportSize(width, height);
        }
    }

    Entity Scene::DuplicateEntity(Entity entity)
    {
        std::string name = entity.GetName();
        Entity newEntity = CreateEntity(name);

        CopyComponentIfExists(AllComponents{}, newEntity, entity);

        return newEntity;
    }

    void Scene::DrawGrid(int size, glm::vec4 color)
    {
        // 3D XZ axis
        Renderer2D::DrawLine(glm::vec3(-size, -1.0f, 0.0f), glm::vec3(size, -1.0f, 0.0), glm::vec4(1, 0, 0, 1));
        Renderer2D::DrawLine(glm::vec3(0.0f, -1.0f, -size), glm::vec3(0.0f, -1.0f, size), glm::vec4(0, 0, 1, 1));

        // TODO: XY axis
        for (int i = 1; i <= size; i++)
        {
            Renderer2D::DrawLine(glm::vec3(0.0f + i, -1.0f, -size), glm::vec3(0.0f + i, -1.0f, size), color);
            Renderer2D::DrawLine(glm::vec3(0.0f - i, -1.0f, -size), glm::vec3(0.0f - i, -1.0f, size), color);
            Renderer2D::DrawLine(glm::vec3(-size, -1.0f, 0.0f - i), glm::vec3(size, -1.0f, 0.0f - i), color);
            Renderer2D::DrawLine(glm::vec3(-size, -1.0f, 0.0f + i), glm::vec3(size, -1.0f, 0.0f + i), color);
        }
    }

    void Scene::SetGrid(int size, glm::vec4 color)
    {
        m_GridSize = size;
        m_GridColor = color;
    }


    Entity Scene::GetEntityWithUUID(UUID uuid)
    {
        if (m_EntityMap.find(uuid) != m_EntityMap.end())
            return {m_EntityMap.at(uuid), this};

        return {};
    }

    Entity Scene::FindEntityByName(std::string_view name)
    {
        auto view = m_Registry.view<TagComponent>();
        for (auto entity : view)
        {
            const TagComponent& tc = view.get<TagComponent>(entity);
            if (tc.Tag == name)
                return Entity{entity, this};
        }

        return {};
    }

    Entity Scene::GetPrimaryCameraEntity()
    {
        auto& view = m_Registry.view<CameraComponent>();
        for (auto entity : view)
        {
            const auto& camera = view.get<CameraComponent>(entity);
            if (camera.Primary)
                return Entity{entity, this};
        }
        return {};
    }

    void Scene::OnPhysics2DStart()
    {
        m_Running = true;
        m_Box2DWorld = new b2World({0.0f, -9.8f});

        auto view = m_Registry.view<Rigidbody2DComponent>();
        for (auto e : view)
        {
            Entity entity = {e, this};
            auto& transform = entity.GetComponent<TransformComponent>();
            auto& rb2d = entity.GetComponent<Rigidbody2DComponent>();

            b2BodyDef bodyDef;
            bodyDef.type = Box2DBodyType(rb2d.Type);
            bodyDef.position.Set(transform.Translation.x, transform.Translation.y);
            bodyDef.angle = transform.Rotation.z;

            b2Body* body = m_Box2DWorld->CreateBody(&bodyDef);
            body->SetFixedRotation(rb2d.FixedRotation);
            rb2d.RuntimeBody = body;

            if (entity.HasComponent<BoxCollider2DComponent>())
            {
                auto& bc2d = entity.GetComponent<BoxCollider2DComponent>();

                b2PolygonShape boxShape;
                boxShape.SetAsBox(bc2d.Size.x * transform.Scale.x, bc2d.Size.y * transform.Scale.y,
                                  b2Vec2(bc2d.Offset.x, bc2d.Offset.y), 0.0f);

                b2FixtureDef fixtureDef;
                fixtureDef.shape = &boxShape;
                fixtureDef.density = bc2d.Density;
                fixtureDef.friction = bc2d.Friction;
                fixtureDef.restitution = bc2d.Restitution;
                fixtureDef.restitutionThreshold = bc2d.RestitutionThreshold;
                body->CreateFixture(&fixtureDef);
            }

            if (entity.HasComponent<CircleCollider2DComponent>())
            {
                auto& cc2d = entity.GetComponent<CircleCollider2DComponent>();

                b2CircleShape circleShape;
                circleShape.m_p.Set(cc2d.Offset.x, cc2d.Offset.y);
                circleShape.m_radius = transform.Scale.x * cc2d.Radius;

                b2FixtureDef fixtureDef;
                fixtureDef.shape = &circleShape;
                fixtureDef.density = cc2d.Density;
                fixtureDef.friction = cc2d.Friction;
                fixtureDef.restitution = cc2d.Restitution;
                fixtureDef.restitutionThreshold = cc2d.RestitutionThreshold;
                body->CreateFixture(&fixtureDef);
            }
        }
    }

    void Scene::OnPhysics2DStop()
    {
        m_Running = false;
        delete m_Box2DWorld;
        m_Box2DWorld = nullptr;
    }

    void Scene::DrawIcon(const EditorCamera& camera, int entity, std::shared_ptr<Texture2D>& texture,
                         TransformComponent& tc, bool rotate)
    {
        glm::mat4 transform = glm::translate(glm::mat4(1.0f), tc.Translation)
            * glm::rotate(glm::mat4(1.0f), -camera.GetYaw(), glm::vec3(0, 1, 0))
            * glm::rotate(glm::mat4(1.0f), -camera.GetPitch(), glm::vec3(1, 0, 0))
            * glm::scale(glm::mat4(1.0f), glm::vec3(1.0f));

        Renderer2D::DrawQuad(transform, texture, 1.0f, glm::vec4(1.0f), (int)entity);
    }

    void Scene::Step(int frames)
    {
        m_StepFrames = frames;
    }

    template <typename T>
    void Scene::OnComponentAdded(Entity entity, T& component)
    {
    }

    template <>
    void Scene::OnComponentAdded<IDComponent>(Entity entity, IDComponent& component)
    {
    }

    template <>
    void Scene::OnComponentAdded<TransformComponent>(Entity entity, TransformComponent& component)
    {
    }

    template <>
    void Scene::OnComponentAdded<AudioComponent>(Entity entity, AudioComponent& component)
    {
    }

    template <>
    void Scene::OnComponentAdded<CameraComponent>(Entity entity, CameraComponent& component)
    {
        if (m_ViewportWidth > 0 && m_ViewportHeight > 0)
            component.Camera.SetViewportSize(m_ViewportWidth, m_ViewportHeight);
    }

    template <>
    void Scene::OnComponentAdded<SpriteRendererComponent>(Entity entity, SpriteRendererComponent& component)
    {
    }

    template <>
    void Scene::OnComponentAdded<PointLightComponent>(Entity entity, PointLightComponent& component)
    {
    }

    template <>
    void Scene::OnComponentAdded<SpotLightComponent>(Entity entity, SpotLightComponent& component)
    {
    }

    template <>
    void Scene::OnComponentAdded<DirectionalLightComponent>(Entity entity, DirectionalLightComponent& component)
    {
    }

    template <>
    void Scene::OnComponentAdded<SpriteRenderer2DComponent>(Entity entity, SpriteRenderer2DComponent& component)
    {
    }

    template <>
    void Scene::OnComponentAdded<StaticMeshComponent>(Entity entity, StaticMeshComponent& component)
    {
    }

    template <>
    void Scene::OnComponentAdded<TextComponent>(Entity entity, TextComponent& component)
    {
    }

    template <>
    void Scene::OnComponentAdded<CircleRendererComponent>(Entity entity, CircleRendererComponent& component)
    {
    }

    template <>
    void Scene::OnComponentAdded<TagComponent>(Entity entity, TagComponent& component)
    {
    }

    template <>
    void Scene::OnComponentAdded<ScriptComponent>(Entity entity, ScriptComponent& component)
    {
    }

    template <>
    void Scene::OnComponentAdded<NativeScriptComponent>(Entity entity, NativeScriptComponent& component)
    {
    }

    template <>
    void Scene::OnComponentAdded<Rigidbody2DComponent>(Entity entity, Rigidbody2DComponent& component)
    {
    }

    template <>
    void Scene::OnComponentAdded<BoxCollider2DComponent>(Entity entity, BoxCollider2DComponent& component)
    {
    }

    template <>
    void Scene::OnComponentAdded<CircleCollider2DComponent>(Entity entity, CircleCollider2DComponent& component)
    {
    }
}
