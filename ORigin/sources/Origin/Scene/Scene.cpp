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

#include "Origin\Animation\Animation.h"

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
      m_CameraIcon = Renderer::GetGTexture("CameraIcon");
      m_LightingIcon = Renderer::GetGTexture("LightingIcon");
      m_AudioIcon = Renderer::GetGTexture("AudioIcon");

      FramebufferSpecification fbSpec;
      fbSpec.Width = 1080;
      fbSpec.Height = 1080;

      fbSpec.WrapMode = GL_CLAMP_TO_BORDER;
      fbSpec.ReadBuffer = false;
      fbSpec.Attachments =
      {
        FramebufferTextureFormat::DEPTH
      };

      shadowFramebuffer = Framebuffer::Create(fbSpec);

			// Create Depth Shader
			m_DepthShader = Shader::Create("Resources/Shaders/DepthMap.glsl");
    }

    Scene::~Scene()
    {
    }

    template <typename... Component>
    static void CopyComponent(entt::registry& dst, entt::registry& src, const std::unordered_map<UUID, entt::entity>& enttMap)
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
                              const std::unordered_map<UUID, entt::entity> enttMap)
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
        std::shared_ptr<Scene> newScene = std::make_shared<Scene>();

        newScene->m_ViewportWidth = other->m_ViewportWidth;
        newScene->m_ViewportHeight = other->m_ViewportHeight;

        entt::registry& srcSceneRegistry = other->m_Registry;
        entt::registry& dstSceneRegistry = newScene->m_Registry;
        std::unordered_map<UUID, entt::entity> enttMap;
        auto newEntity = Entity();

        // create entities in new scene
        auto idView = srcSceneRegistry.view<IDComponent>();
        for (auto e : idView)
        {
            UUID uuid = srcSceneRegistry.get<IDComponent>(e).ID;

            const auto name = srcSceneRegistry.get<TagComponent>(e).Tag;
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

    Entity Scene::CreatePointlight()
    {
        Entity entity = {m_Registry.create(), this};
        entity.AddComponent<IDComponent>(UUID());
        entity.AddComponent<TransformComponent>();
        entity.AddComponent<PointLightComponent>();

        auto& tag = entity.AddComponent<TagComponent>();
        tag.Tag = "Point Light";

        UUID& uuid = entity.GetComponent<IDComponent>().ID;
        entity.GetComponent<TransformComponent>().Translation.y = 5.0f;

        m_EntityMap.insert(std::make_pair(uuid, entity));

        return entity;
    }

    Entity Scene::CreateSpotLight()
    {
        Entity entity = {m_Registry.create(), this};
        entity.AddComponent<IDComponent>(UUID());
        entity.AddComponent<TransformComponent>();
        entity.AddComponent<SpotLightComponent>();

        auto& tag = entity.AddComponent<TagComponent>();
        tag.Tag = "Spot Light";

        UUID& uuid = entity.GetComponent<IDComponent>().ID;
        entity.GetComponent<TransformComponent>().Translation.y = 3.0f;

        m_EntityMap.insert(std::make_pair(uuid, entity));

        return entity;
    }

    Entity Scene::CreateDirectionalLight()
    {
      Entity entity = { m_Registry.create(), this };
      entity.AddComponent<IDComponent>(UUID());
      entity.AddComponent<TransformComponent>();
      entity.AddComponent<DirectionalLightComponent>();

      auto& tag = entity.AddComponent<TagComponent>();
      tag.Tag = "Directional Light";

      UUID& uuid = entity.GetComponent<IDComponent>().ID;
      entity.GetComponent<TransformComponent>().Rotation.x = glm::radians(-90.0f);

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

    
    void Scene::OnUpdateRuntime(Timestep deltaTime)
    {
      if (!m_Paused || m_StepFrames-- > 0)
      {
        // Update Scripts
        auto& scriptView = m_Registry.view<ScriptComponent>();
        for (auto& e : scriptView)
        {
          Entity entity = { e, this };
          ScriptEngine::OnUpdateEntity(entity, deltaTime);
        }

        m_Registry.view<NativeScriptComponent>().each([=](auto entity, auto& nsc)
        {
          if (!nsc.Instance)
          {
            nsc.Instance = nsc.InstantiateScript();
            nsc.Instance->m_Entity = Entity{ entity, this };
            nsc.Instance->OnCreate();
          }
          nsc.Instance->OnUpdate(deltaTime);
        });

        // Physics
        {
          const int32_t velocityIterations = 6;
          const int32_t positionIterations = 2;
          m_Box2DWorld->Step(deltaTime, velocityIterations, positionIterations);

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

      // Particle Update
      m_Registry.view<Particle2DComponent>().each([=](auto entity, auto& pc)
      {
        pc.Particle.OnUpdate(deltaTime);
      });

      // Audio Update
      auto audioView = m_Registry.view<TransformComponent, AudioComponent>();
      for (auto entity : audioView)
      {
        auto& [tc, ac] = audioView.get<TransformComponent, AudioComponent>(entity);
        if (ac.Audio)
        {
          ac.Audio->SetMaxDistance(ac.MaxDistance);
          ac.Audio->SetMinDistance(ac.MinDistance);
          ac.Audio->SetLoop(ac.Looping);
          ac.Audio->SetGain(ac.Volume);
          ac.Audio->SetPosition(tc.Translation);
        }
      }

      auto audioListenerView = m_Registry.view<TransformComponent, AudioListenerComponent>();
      for (auto entity : audioListenerView)
      {
        auto& [tc, al] = audioListenerView.get<TransformComponent, AudioListenerComponent>(entity);
        if (al.Enable)
          al.Listener.Set(tc.Translation, glm::vec3(1.0f), tc.GetForward(), tc.GetUp());
        AudioEngine::SetMute(!al.Enable);
      }

      AudioEngine::SystemUpdate();
    }

    void Scene::OnUpdateEditor(Timestep deltaTime, EditorCamera& editorCamera)
    {
      m_Registry.view<NativeScriptComponent>().each([=](auto entity, auto& nsc)
      {
          if (!nsc.Instance)
          {
              nsc.Instance = nsc.InstantiateScript();
              nsc.Instance->m_Entity = Entity{entity, this};
              nsc.Instance->OnCreate();
          }
          nsc.Instance->OnUpdate(deltaTime);
      });

      //Render
      RenderScene(editorCamera);
      {
        m_Registry.view<Particle2DComponent>().each([=](auto entity, auto& pc)
        {
            pc.Particle.OnUpdate(deltaTime);
        });
      }

      // Animation
      auto animView = m_Registry.view<AnimationComponent>();
      for (auto entity : animView)
      {
        auto& ac = animView.get<AnimationComponent>(entity);
        if (ac.Animation)
          ac.Animation->Update(deltaTime);
      }

      // Audio Update
      Renderer2D::BeginScene();
      auto audioView = m_Registry.view<TransformComponent, AudioComponent>();
      for (auto entity : audioView)
      {
        auto& [tc, ac] = audioView.get<TransformComponent, AudioComponent>(entity);
        if (ac.Audio)
        {
          ac.Audio->SetMaxDistance(ac.MaxDistance);
          ac.Audio->SetMinDistance(ac.MinDistance);
          ac.Audio->SetLoop(ac.Looping);
          ac.Audio->SetGain(ac.Volume);
          ac.Audio->SetPosition(tc.Translation);
        }

        DrawIcon(editorCamera, (int)entity, m_AudioIcon, tc, true);
      }
      Renderer2D::EndScene();

      editorCamera.UpdateAudioListener();
      AudioEngine::SetMute(false);
      AudioEngine::SystemUpdate();
    }

    void Scene::OnUpdateSimulation(Timestep deltaTime, EditorCamera& editorCamera)
    {
      if (!m_Paused || m_StepFrames-- > 0)
      {
        // Update Scripts
        {
          auto& view = m_Registry.view<ScriptComponent>();
          for (auto e : view)
          {
              Entity entity = {e, this};
              ScriptEngine::OnUpdateEntity(entity, deltaTime);
          }

          m_Registry.view<NativeScriptComponent>().each([=](auto entity, auto& nsc)
          {
              if (!nsc.Instance)
              {
                  nsc.Instance = nsc.InstantiateScript();
                  nsc.Instance->m_Entity = Entity{entity, this};
                  nsc.Instance->OnCreate();
              }
              nsc.Instance->OnUpdate(deltaTime);
          });
        }

        // Physics
        {
          const int32_t velocityIterations = 6;
          const int32_t positionIterations = 2;
          m_Box2DWorld->Step(deltaTime, velocityIterations, positionIterations);

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
      RenderScene(editorCamera);

      {
        m_Registry.view<Particle2DComponent>().each([=](auto entity, auto& pc)
          {
            pc.Particle.OnUpdate(deltaTime);
          });
      }

      // Audio Update
      Renderer2D::BeginScene();
      AudioEngine::SetMute(false);
      auto audioView = m_Registry.view<TransformComponent, AudioComponent>();
      for (auto entity : audioView)
      {
        auto& [tc, ac] = audioView.get<TransformComponent, AudioComponent>(entity);
        if (ac.Audio)
        {
          ac.Audio->SetMaxDistance(ac.MaxDistance);
          ac.Audio->SetMinDistance(ac.MinDistance);
          ac.Audio->SetLoop(ac.Looping);
          ac.Audio->SetGain(ac.Volume);

          if(ac.Spatial)
            ac.Audio->SetPosition(tc.Translation);
        }

        DrawIcon(editorCamera, (int)entity, m_AudioIcon, tc, true);
      }
      Renderer2D::EndScene();

      bool isMainCameraListening = false;
      auto audioListenerView = m_Registry.view<TransformComponent, AudioListenerComponent>();
      for (auto entity : audioListenerView)
      {
        auto& [tc, al] = audioListenerView.get<TransformComponent, AudioListenerComponent>(entity);
        if (al.Enable)
          al.Listener.Set(tc.Translation, glm::vec3(1.0f), tc.GetForward(), tc.GetUp());

        isMainCameraListening = al.Enable;
      }

      if (!isMainCameraListening)
        editorCamera.UpdateAudioListener();
      
      AudioEngine::SystemUpdate();
    }

    void Scene::OnSimulationStart()
    {
      OnPhysics2DStart();

      // Scripting
      ScriptEngine::OnRuntimeStart(this);
      auto scriptView = m_Registry.view<ScriptComponent>();
      for (auto e : scriptView)
      {
        Entity entity = { e, this };
        ScriptEngine::OnCreateEntity(entity);
      }

      // Audio
      auto audioView = m_Registry.view<AudioComponent>();
      for (auto& e : audioView)
      {
        auto& ac = audioView.get<AudioComponent>(e);
        if (ac.Audio && ac.PlayAtStart)
          ac.Audio->Play();
      }
    }

    void Scene::OnSimulationStop()
    {
        OnPhysics2DStop();
        ScriptEngine::OnRuntimeStop();

        // Audio
        auto view = m_Registry.view<AudioComponent>();
        for (auto& e : view)
        {
          auto& ac = view.get<AudioComponent>(e);
          if (ac.Audio)
            ac.Audio->Stop();
        }
    }

    void Scene::RenderScene(Camera* camera, const TransformComponent& cameraTransform)
    {
        Renderer::BeginScene(*camera, cameraTransform.GetTransform());
        // Get Camera Position
        const glm::vec3& MainCameraPosition = cameraTransform.Translation;

        // Particle
        {
          auto& view = m_Registry.view<TransformComponent, Particle2DComponent>();
          for (auto entity : view)
          {
            auto& [tc, pc] = view.get<TransformComponent, Particle2DComponent>(entity);

            for (int i = 0; i < 5; i++)
              pc.Particle.Emit(pc, glm::vec3(tc.Translation.x, tc.Translation.y, tc.Translation.z + pc.ZAxis), (int)entity);

            pc.Particle.OnRender();
          }
        }

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

				// Mesh
				auto& mehsView = m_Registry.view<TransformComponent, StaticMeshComponent>();
				for (auto& entity : mehsView)
				{
					auto& [tc, mesh] = mehsView.get<TransformComponent, StaticMeshComponent>(entity);
					if (mesh.Model)
					{
					}
				}

        Renderer::EndScene();
    }

    void Scene::OnShadowRender()
    {
      m_DepthShader->Enable();

      auto dirLight = m_Registry.view<TransformComponent, DirectionalLightComponent>();
      for (auto light : dirLight)
      {
        auto [tc, lc] = dirLight.get<TransformComponent, DirectionalLightComponent>(light);
				glm::mat4 lightProjection = glm::ortho(-ShadowSize, ShadowSize,
					-ShadowSize, ShadowSize, -NearFar, NearFar);

				// direction/eye, center, up
				glm::mat4 lightView = glm::lookAt(glm::radians(-tc.GetForward()), glm::vec3(0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
        m_LightSpaceMatrix = lightProjection * lightView;
				m_DepthShader->SetMatrix("uLightSpaceMatrix", m_LightSpaceMatrix);
      }

      glCullFace(GL_FRONT);
      shadowFramebuffer->Bind();
      glClear(GL_DEPTH_BUFFER_BIT);
		  // Mesh
		  auto& meshView = m_Registry.view<TransformComponent, StaticMeshComponent>();
		  for (auto& entity : meshView)
		  {
			  auto& [tc, mesh] = meshView.get<TransformComponent, StaticMeshComponent>(entity);

        if (mesh.Model)
        {
          // Set Depth shader
          m_DepthShader->SetMatrix("uModel", tc.GetTransform());
          mesh.Model->Draw();
        }
      }

      m_DepthShader->Disable();
      shadowFramebuffer->Unbind();
      glCullFace(GL_BACK);

    }

    void Scene::RenderScene(const EditorCamera& camera)
    {
      Renderer::BeginScene(camera);

      // Particle
      {
        auto& view = m_Registry.view<TransformComponent, Particle2DComponent>();
        for (auto entity : view)
        {
          auto& [tc, pc] = view.get<TransformComponent, Particle2DComponent>(entity);

          for (int i = 0; i < 5; i++)
            pc.Particle.Emit(pc, glm::vec3(tc.Translation.x, tc.Translation.y, tc.Translation.z + pc.ZAxis), (int)entity);

          pc.Particle.OnRender();
        }
      }

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

          auto& view2 = m_Registry.view<SpriteRenderer2DComponent, AnimationComponent>();
          for (auto& entity : view2)
          {
            auto& [sprite, anim] = view2.get<SpriteRenderer2DComponent, AnimationComponent>(entity);
            if (anim.State.HasAnimation())
              sprite.Texture = anim.State.GetAnimation()->GetCurrentSprite();
          }
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
			auto& cameraView = m_Registry.view<TransformComponent, CameraComponent>();
			for (auto& entity : cameraView)
			{
				auto& [tc, cc] = cameraView.get<TransformComponent, CameraComponent>(entity);
				DrawIcon(camera, (int)entity, m_CameraIcon, tc, true);
			}

      // Lighting
      auto& pointLightView = m_Registry.view<TransformComponent, PointLightComponent>();
      for (auto& pointLight : pointLightView)
      {
        auto& [tc, lc] = pointLightView.get<TransformComponent, PointLightComponent>(pointLight);
        DrawIcon(camera, (int)pointLight, m_LightingIcon, tc, true);
      }

      auto& spotLightView = m_Registry.view<TransformComponent, SpotLightComponent>();
      for (auto& spotLight : spotLightView)
      {
        auto& [tc, lc] = spotLightView.get<TransformComponent, SpotLightComponent>(spotLight);
        DrawIcon(camera, (int)spotLight, m_LightingIcon, tc, true);
      }

      auto& dirLightView = m_Registry.view<TransformComponent, DirectionalLightComponent>();
      for (auto& dirLight : dirLightView)
      {
        auto& [tc, lc] = dirLightView.get<TransformComponent, DirectionalLightComponent>(dirLight);
        DrawIcon(camera, (int)dirLight, m_LightingIcon, tc, true);
      }

			// Mesh
			auto& meshView = m_Registry.view<TransformComponent, StaticMeshComponent>();
			for (auto& entity : meshView)
			{
				auto& [tc, mesh] = meshView.get<TransformComponent, StaticMeshComponent>(entity);

				int pointLightCount = 0;
				int spotLightCount = 0;

        if (mesh.Model)
        {
          std::string lightUniformName;
					mesh.Material->EnableShader();

          mesh.Material->SetMatrix("uLightSpaceMatrix", glm::mat4(1.0f));

          for (auto& pointLight : pointLightView)
          {
            auto& [tc, lc] = pointLightView.get<TransformComponent, PointLightComponent>(pointLight);

            lightUniformName = std::string("pointLights[" + std::to_string(pointLightCount) + "].");
            mesh.Material->SetInt("pointLightCount", pointLightCount);
            mesh.Material->SetVector(lightUniformName + "Position", tc.Translation);
            mesh.Material->SetVector(lightUniformName + "Color", lc.Color);
            mesh.Material->SetFloat(lightUniformName + "Ambient", lc.Ambient);
            mesh.Material->SetFloat(lightUniformName + "Specular", lc.Specular);

            pointLightCount++;
          }

					for (auto& spotLight : spotLightView)
					{
						auto& [tc, lc] = spotLightView.get<TransformComponent, SpotLightComponent>(spotLight);

						lightUniformName = std::string("spotLights[" + std::to_string(spotLightCount) + "].");
              
            mesh.Material->SetInt("spotLightCount", spotLightCount);
						mesh.Material->SetVector(lightUniformName + "Position", tc.Translation);
            mesh.Material->SetVector(lightUniformName + "Direction", -tc.GetForward());
						mesh.Material->SetVector(lightUniformName + "Color", lc.Color);
						mesh.Material->SetFloat(lightUniformName + "InnerConeAngle", lc.InnerConeAngle);
						mesh.Material->SetFloat(lightUniformName + "OuterConeAngle", lc.OuterConeAngle);
						mesh.Material->SetFloat(lightUniformName + "Exponent", lc.Exponent);

						spotLightCount++;
					}

          for (auto& dirLight : dirLightView)
          {
            auto& [tc, lc] = dirLightView.get<TransformComponent, DirectionalLightComponent>(dirLight);
            
            mesh.Material->SetMatrix("uLightSpaceMatrix", m_LightSpaceMatrix);
            mesh.Material->SetVector("dirLight.Direction", -tc.GetForward());
            mesh.Material->SetVector("dirLight.Color", lc.Color);
            mesh.Material->SetFloat("dirLight.Ambient", lc.Ambient);
            mesh.Material->SetFloat("dirLight.Diffuse", lc.Diffuse);
            mesh.Material->SetFloat("dirLight.Specular", lc.Specular);
          }

          glActiveTexture(shadowFramebuffer->GetDepthAttachmentRendererID());
          glBindTexture(GL_TEXTURE_2D, shadowFramebuffer->GetDepthAttachmentRendererID());
          mesh.Material->SetInt("uShadowMap", 0);

					mesh.Model->Draw(tc.GetTransform(), camera, static_cast<int>(entity));

          mesh.Material->DisableShader();

          if (mesh.Material->m_PointLightCount != pointLightCount || mesh.Material->m_SpotLightCount != spotLightCount)
            mesh.Material->RefreshShader();

          mesh.Material->m_PointLightCount = pointLightCount;
          mesh.Material->m_SpotLightCount = spotLightCount;
        }
			}

      //DrawGrid(m_GridSize, m_GridColor);
      Renderer::EndScene();
    }

    void Scene::OnRuntimeStart()
    {
        OnPhysics2DStart();

        // Scripting
        ScriptEngine::OnRuntimeStart(this);
        auto scriptView = m_Registry.view<ScriptComponent>();
        for (auto e : scriptView)
        {
          Entity entity = { e, this };
          ScriptEngine::OnCreateEntity(entity);
        }

        // Audio
        auto audioView = m_Registry.view<AudioComponent>();
        for (auto& e : audioView)
        {
          auto& ac = audioView.get<AudioComponent>(e);
          if (ac.Audio && ac.PlayAtStart)
          {
            ac.Audio->SetGain(ac.Volume);
            ac.Audio->Play();
          }
        }
    }

    void Scene::OnRuntimeStop()
    {
        OnPhysics2DStop();
        ScriptEngine::OnRuntimeStop();

        // Audio
        auto audioView = m_Registry.view<AudioComponent>();
        for (auto& e : audioView)
        {
          auto& ac = audioView.get<AudioComponent>(e);
          if (ac.Audio)
            ac.Audio->Stop();
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

#define OGN_REG_COMPONENT(components)\
template<>\
void Scene::OnComponentAdded<components>(Entity entity, components& component){}

    OGN_REG_COMPONENT(IDComponent)
    OGN_REG_COMPONENT(TransformComponent)
    OGN_REG_COMPONENT(AudioComponent)
    OGN_REG_COMPONENT(AudioListenerComponent)
    OGN_REG_COMPONENT(AnimationComponent)
    OGN_REG_COMPONENT(SpriteRendererComponent)
    OGN_REG_COMPONENT(SpriteRenderer2DComponent)
    OGN_REG_COMPONENT(PointLightComponent)
    OGN_REG_COMPONENT(SpotLightComponent)
    OGN_REG_COMPONENT(DirectionalLightComponent)
    OGN_REG_COMPONENT(StaticMeshComponent)
    OGN_REG_COMPONENT(TextComponent)
    OGN_REG_COMPONENT(CircleRendererComponent)
    OGN_REG_COMPONENT(TagComponent)
    OGN_REG_COMPONENT(NativeScriptComponent)
    OGN_REG_COMPONENT(ScriptComponent)
    OGN_REG_COMPONENT(Rigidbody2DComponent)
    OGN_REG_COMPONENT(BoxCollider2DComponent)
    OGN_REG_COMPONENT(CircleCollider2DComponent)
    OGN_REG_COMPONENT(Particle2DComponent)

    template <>
    void Scene::OnComponentAdded<CameraComponent>(Entity entity, CameraComponent& component)
    {
        if (m_ViewportWidth > 0 && m_ViewportHeight > 0)
            component.Camera.SetViewportSize(m_ViewportWidth, m_ViewportHeight);
    }
}
