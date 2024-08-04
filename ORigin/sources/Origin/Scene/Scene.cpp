// Copyright (c) Evangelion Manuhutu | ORigin Engine

#include "pch.h"
#include "Scene.h"

#include "Origin/Audio/AudioEngine.h"
#include "Origin/Audio/AudioSource.h"
#include "Origin/Profiler/Profiler.h"
#include "Origin/Animation/Animation.h"
#include "Origin/Physics/2D/Physics2D.h"
#include "Origin/Physics/2D/Contact2DListener.h"
#include "Origin/Renderer/Renderer.h"
#include "Origin/Renderer/Renderer2D.h"
#include "Origin/Renderer/MeshRenderer.h"
#include "Origin/Scripting/ScriptEngine.h"
#include "Origin/Asset/AssetManager.h"
#include "Origin/Core/Log.h"
#include "Origin/Core/Input.h"

#include "EntityManager.h"
#include "ScriptableEntity.h"
#include "Lighting.h"
#include "Entity.h"

#include "Origin/Physics/PhysicsEngine.h"

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/quaternion.hpp>
#include <glm/gtx/compatibility.hpp>
#include <glm/glm.hpp>

namespace origin
{

	Scene::Scene()
	{
		OGN_PROFILER_SCENE();
		m_Physics2D = std::make_shared<Physics2D>(this);
		m_UIRenderer = std::make_shared<UIRenderer>();
	}

	std::shared_ptr<Scene> Scene::Copy(const std::shared_ptr<Scene> &other)
	{
		OGN_PROFILER_SCENE();

		auto newScene = std::make_shared<Scene>();
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

			enttStorage.push_back({ idc.ID, static_cast<entt::entity>(newEntity) });
		}

		EntityManager::CopyComponent(AllComponents {}, dstSceneRegistry, srcSceneRegistry, enttStorage);
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

		return Entity { entt::null, nullptr };
	}

	Entity Scene::FindEntityByName(std::string_view name)
	{
		OGN_PROFILER_SCENE();

		auto view = m_Registry.view<TagComponent>();
		for (auto entity : view)
		{
			const TagComponent &tc = view.get<TagComponent>(entity);
			if (tc.Tag == name)
				return { entity, this };
		}

		return Entity();
	}

    void Scene::Render3DScene(const Camera &camera, entt::entity selectedId)
    {
        glEnable(GL_DEPTH_TEST);
        glEnable(GL_STENCIL_TEST);
        glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);

        const auto &lightView = m_Registry.view<TransformComponent, LightComponent>();
        const auto &meshView = m_Registry.view<TransformComponent, StaticMeshComponent>();

        // First pass: Render all objects and mark the selected object in the stencil buffer
        for (auto &li : lightView)
        {
            const auto &[lightTC, lc] = lightView.get<TransformComponent, LightComponent>(li);
            if (!lightTC.Visible)
                continue;

            lc.Light->DirLightData.LightSpaceMat = lc.Light->GetShadow().ViewProjection;
            lc.Light->DirLightData.Position = glm::vec4(lightTC.WorldTranslation, 1.0f);
            lc.Light->LightingUBO->Bind();
            lc.Light->LightingUBO->SetData(&lc.Light->DirLightData, sizeof(lc.Light->DirLightData));
            MeshRenderer::AttachShadow(lc.Light->GetShadow().DepthMapID);

            for (auto e : meshView)
            {
                const auto [tc, mc] = meshView.get<TransformComponent, StaticMeshComponent>(e);
                if (!tc.Visible)
                    continue;

                std::shared_ptr<Material> material;
                if (mc.HMaterial) material = AssetManager::GetAsset<Material>(mc.HMaterial);
                else material = Renderer::GetMaterial("Mesh");

                // Set stencil function based on whether this is the selected entity
                if (e == selectedId)
                {
                    glStencilFunc(GL_ALWAYS, 1, 0xFF);
                    glStencilMask(0xFF);
                }
                else
                {
                    glStencilFunc(GL_ALWAYS, 0, 0xFF);
                    glStencilMask(0x00);
                }

                MeshRenderer::Begin(camera);
                switch (mc.mType)
                {
                case StaticMeshComponent::Type::Cube:
                    MeshRenderer::DrawCube(tc.GetTransform(), material.get(), (int)e);
                    break;
                case StaticMeshComponent::Type::Sphere:
                    MeshRenderer::DrawSphere(tc.GetTransform(), material.get(), (int)e);
                    break;
                case StaticMeshComponent::Type::Capsule:
                    MeshRenderer::DrawCapsule(tc.GetTransform(), material.get(), (int)e);
                    break;
                }
                MeshRenderer::End();
            }
        }

        // Second pass: Draw the outline for the selected object
        if (m_Registry.valid(selectedId))
        {
			Entity entity = { selectedId, this };
			if (entity.HasComponent<StaticMeshComponent>())
			{
				glStencilFunc(GL_NOTEQUAL, 1, 0xFF);
				glStencilMask(0x00);
                glEnable(GL_DEPTH_TEST);  // Enable depth testing
                glDepthFunc(GL_LEQUAL);   // Change depth function to allow drawing on top of the object

				Shader *outlineShader = Renderer::GetShader("Outline").get();
				outlineShader->Enable();
				outlineShader->SetMatrix("viewProjection", camera.GetViewProjection());

				MeshRenderer::Begin(camera, outlineShader);

				const auto [tc, mc] = meshView.get<TransformComponent, StaticMeshComponent>(selectedId);

				// Calculate distance-based outline thickness
				glm::vec3 cameraPosition = camera.GetPosition();
				glm::vec3 objectPosition = tc.WorldTranslation;
				float distance = glm::distance(cameraPosition, objectPosition);

				// Calculate the object's bounding sphere radius (approximation)
				float objectRadius = glm::length(tc.WorldScale) * 0.5f;

				// Calculate the outline thickness
				float baseThickness = 0.02f; // Adjust this value to change the base thickness
				float thicknessFactor = baseThickness * (distance / objectRadius);

				// Clamp the thickness factor to a reasonable range
				thicknessFactor = glm::clamp(thicknessFactor, 0.001f, 0.1f);

				glm::mat4 trA = glm::translate(glm::mat4(1.0f), tc.WorldTranslation)
					* glm::toMat4(tc.WorldRotation)
					* glm::scale(glm::mat4(1.0f), tc.WorldScale * (1.0f + thicknessFactor));

				switch (mc.mType)
				{
				case StaticMeshComponent::Type::Cube:
					MeshRenderer::DrawCube(trA, glm::vec4(1.0f), (int)selectedId);
					break;
				case StaticMeshComponent::Type::Sphere:
					MeshRenderer::DrawSphere(trA, glm::vec4(1.0f), (int)selectedId);
					break;
				case StaticMeshComponent::Type::Capsule:
					MeshRenderer::DrawCapsule(trA, glm::vec4(1.0f), (int)selectedId);
					break;
				}

				MeshRenderer::End();
				outlineShader->Disable();

				// Reset state
				glStencilMask(0xFF);
				glStencilFunc(GL_ALWAYS, 0, 0xFF);
				glEnable(GL_DEPTH_TEST);
			}
        }

        glDisable(GL_STENCIL_TEST);
    }

    void Scene::UpdateRuntime(Timestep ts)
	{
		if (!m_Paused || m_StepFrames-- > 0)
		{
			// Update Scripts
			auto scriptView = m_Registry.view<ScriptComponent>();
			for (auto e : scriptView)
			{
				Entity entity = { e, this };
				if (entity)
				{
					ScriptEngine::OnUpdateEntity(entity, ts);
				}
			}

			m_Registry.view<NativeScriptComponent>().each([time = ts](auto entity, auto &nsc)
			{
				if (nsc.Instance)
				{
					nsc.Instance->OnUpdate(time);
				}
			});

			// Particle Update
			m_Registry.view<ParticleComponent>().each([this, time = ts](auto entity, auto &pc)
			{
				pc.Particle.OnUpdate(time);
			});


			// Animation
			const auto &animView = m_Registry.view<SpriteAnimationComponent>();
			for (const auto entity : animView)
			{
				auto &ac = animView.get<SpriteAnimationComponent>(entity);
				if (ac.State->IsCurrentAnimationExists())
				{
					ac.State->OnUpdateRuntime(ts);
				}
			}

			// Audio Update
			const auto &audioView = m_Registry.view<TransformComponent, AudioComponent>();
			for (auto entity : audioView)
			{
				auto [tc, ac] = audioView.get<TransformComponent, AudioComponent>(entity);

				if (std::shared_ptr<AudioSource> audio = AssetManager::GetAsset<AudioSource>(ac.Audio))
				{
					audio->SetVolume(ac.Volume);
					audio->SetPitch(ac.Pitch);
					audio->SetPaning(ac.Panning);
					audio->SetLoop(ac.Looping);
					audio->SetSpatial(ac.Spatializing);
					if (ac.Spatializing)
					{
						audio->SetMinMaxDistance(ac.MinDistance, ac.MaxDistance);
						audio->SetPosition(tc.Translation);
					}
				}
			}

			const auto &audioListenerView = m_Registry.view<TransformComponent, AudioListenerComponent>();
			for (const auto entity : audioListenerView)
			{
				auto [tc, al] = audioListenerView.get<TransformComponent, AudioListenerComponent>(entity);
				if (al.Enable)
				{
					al.Listener.Set(tc.Translation, glm::vec3(0.0f), tc.GetForward(), tc.GetUp());
				}
			}

            // Update entire transform hierarchy
            UpdateTransform();

			PhysicsEngine::Simulate(ts, this);
			m_Physics2D->Simulate(ts);
		}
	}

    void Scene::DestroyEntityRecursive(UUID entityId)
    {
		std::vector<UUID> childrenIds = GetChildrenUUIDs(entityId);
		for (const auto &childId : childrenIds)
		{
			DestroyEntityRecursive(childId);
		}
		Entity entity = GetEntityWithUUID(entityId);
		if (entity)
		{
			m_Registry.destroy((entt::entity)entity);
            m_EntityStorage.erase(std::remove_if(m_EntityStorage.begin(), m_EntityStorage.end(),
                [entityId](const auto &pair) { return pair.first == entityId; }),
           m_EntityStorage.end());
		}
    }

    void Scene::DestroyEntity(Entity entity)
	{
        UUID uuid = entity.GetUUID();
        DestroyEntityRecursive(uuid);
	}

    void Scene::DeserializeDeletedEntity()
    {

    }

    Entity Scene::DuplicateEntityRecursive(Entity entity, Entity newParent)
    {
		std::string name = entity.GetTag();
		Entity newEntity = EntityManager::CreateEntity(name, this, entity.GetType());
		EntityManager::CopyComponentIfExists(AllComponents {}, newEntity, entity);

		auto &newEntityIDC = newEntity.GetComponent<IDComponent>();
		if (newParent)
		{
			newEntityIDC.Parent = newParent.GetUUID();
		}
		else if (entity.HasParent())
		{
			newEntityIDC.Parent = entity.GetParentUUID();
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

    std::vector<origin::UUID> Scene::GetChildrenUUIDs(UUID parentId)
    {
        std::vector<UUID> childrenUUIDs;
        m_Registry.view<IDComponent>().each([&](auto entity, const IDComponent idc)
        {
            if (idc.Parent == parentId)
            {
                childrenUUIDs.push_back(idc.ID);
            }
        });
        return childrenUUIDs;
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

		return {entt::null, nullptr};
	}

	void Scene::OnUpdateRuntime(Timestep ts)
	{
		OGN_PROFILER_SCENE();

		UpdateRuntime(ts);

		// Rendering
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
		auto scriptView = m_Registry.view<ScriptComponent>();
		for (auto e : scriptView)
		{
			Entity entity = { e, this };
			ScriptEngine::OnCreateEntity(entity);
		}

		m_Registry.view<NativeScriptComponent>().each([this](auto entity, auto &nsc)
		{
			if (!nsc.Instance)
			{
				nsc.Instance = nsc.InstantiateScript();
				nsc.Instance->m_Entity = Entity { entity, this };
			}
		});

		auto audioView = m_Registry.view<AudioComponent>();
		for (auto &e : audioView)
		{
			auto &ac = audioView.get<AudioComponent>(e);
			const std::shared_ptr<AudioSource> &audio = AssetManager::GetAsset<AudioSource>(ac.Audio);
			if (ac.PlayAtStart)
			{
				audio->Play();
			}
		}

		PhysicsEngine::OnSimulateStart(this);
		m_Physics2D->OnSimulationStart();

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
			if (const std::shared_ptr<AudioSource> &audio = AssetManager::GetAsset<AudioSource>(ac.Audio))
			{
				audio->Stop();
			}
		}

		PhysicsEngine::OnSimulateStop(this);
		m_Physics2D->OnSimulationStop();

		m_UIRenderer->Unload();
	}

	void Scene::OnEditorUpdate(Timestep ts, Camera &editorCamera)
	{
		OGN_PROFILER_RENDERING();

		m_Registry.view<NativeScriptComponent>().each([time = ts](auto entity, auto &nsc)
		{
			nsc.Instance->OnUpdate(time);
		});

		m_Registry.view<ParticleComponent>().each([time = ts](auto entity, auto &pc)
		{
			pc.Particle.OnUpdate(time);
		});

		// Animation
		const auto &animView = m_Registry.view<SpriteAnimationComponent>();
		for (const auto entity : animView)
		{
			auto &ac = animView.get<SpriteAnimationComponent>(entity);
			if (ac.State->IsCurrentAnimationExists())
				ac.State->OnUpdateEditor(ts);
		}

		// Audio Update
		const auto &audioView = m_Registry.view<TransformComponent, AudioComponent>();
		for (auto entity : audioView)
		{
			const auto &[tc, ac] = audioView.get<TransformComponent, AudioComponent>(entity);
			if (const std::shared_ptr<AudioSource> &audio = AssetManager::GetAsset<AudioSource>(ac.Audio))
			{
				audio->SetVolume(ac.Volume);
				audio->SetPitch(ac.Pitch);
				audio->SetPaning(ac.Panning);
				audio->SetLoop(ac.Looping);
				audio->SetSpatial(ac.Spatializing);
				if (ac.Spatializing)
				{
					audio->SetMinMaxDistance(ac.MinDistance, ac.MaxDistance);
					audio->SetPosition(tc.Translation);
				}
			}
		}

        // Update entire transform hierarchy
        UpdateTransform();

		RenderScene(editorCamera);
	}

	void Scene::OnUpdateSimulation(Timestep ts, Camera &editorCamera)
	{
		OGN_PROFILER_RENDERING();

		if (!m_Paused || m_StepFrames-- > 0)
		{
			// Update Scripts
			m_Registry.view<ScriptComponent>().each([this, time = ts](auto entityID, auto &sc)
			{
				Entity entity { entityID, this };
				ScriptEngine::OnUpdateEntity(entity, time);
			});

			m_Registry.view<NativeScriptComponent>().each([this, time = ts](auto entityID, auto &nsc)
			{
				nsc.Instance->OnUpdate(time);
			});

			m_Registry.view<ParticleComponent>().each([this, time = ts](auto entity, auto &pc)
			{
				pc.Particle.OnUpdate(time);
			});

			// Animation
			const auto &animView = m_Registry.view<SpriteAnimationComponent>();
			for (auto e : animView)
			{
				auto ac = animView.get<SpriteAnimationComponent>(e);
				if (ac.State->IsCurrentAnimationExists())
					ac.State->OnUpdateRuntime(ts);
			}

			Renderer2D::Begin(editorCamera);
			const auto &audioView = m_Registry.view<TransformComponent, AudioComponent>();
			for (auto entity : audioView)
			{
				const auto &[tc, ac] = audioView.get<TransformComponent, AudioComponent>(entity);
				if (const std::shared_ptr<AudioSource> &audio = AssetManager::GetAsset<AudioSource>(ac.Audio))
				{
					audio->SetVolume(ac.Volume);
					audio->SetPitch(ac.Pitch);
					audio->SetPaning(ac.Panning);
					audio->SetLoop(ac.Looping);
					audio->SetSpatial(ac.Spatializing);
					if (ac.Spatializing)
					{
						audio->SetMinMaxDistance(ac.MinDistance, ac.MaxDistance);
						audio->SetPosition(tc.Translation);
					}
				}
			}
			Renderer2D::End();

			const auto &audioListenerView = m_Registry.view<TransformComponent, AudioListenerComponent>();
			for (const auto entity : audioListenerView)
			{
				auto [tc, al] = audioListenerView.get<TransformComponent, AudioListenerComponent>(entity);
				if (al.Enable)
					al.Listener.Set(tc.Translation, glm::vec3(0.0f), tc.GetForward(), tc.GetUp());
			}

			PhysicsEngine::Simulate(ts, this);
			m_Physics2D->Simulate(ts);
		}

        // Update entire transform hierarchy
        UpdateTransform();
		
		RenderScene(editorCamera);
	}

	void Scene::OnSimulationStart()
	{
		OGN_PROFILER_SCENE();

		m_Running = true;

		ScriptEngine::SetSceneContext(this);
		const auto &scriptView = m_Registry.view<ScriptComponent>();
		for (auto e : scriptView)
		{
			Entity entity = { e, this };
			ScriptEngine::OnCreateEntity(entity);
		}

		m_Registry.view<NativeScriptComponent>().each([this](auto entity, auto &nsc)
		{
			nsc.Instance = nsc.InstantiateScript();
			nsc.Instance->m_Entity = Entity { entity, this };
		});

		PhysicsEngine::OnSimulateStart(this);
		m_Physics2D->OnSimulationStart();

		// Audio
		const auto &audioView = m_Registry.view<AudioComponent>();
		for (auto &e : audioView)
		{
			auto &ac = audioView.get<AudioComponent>(e);
			const std::shared_ptr<AudioSource> &audio = AssetManager::GetAsset<AudioSource>(ac.Audio);
			if (ac.PlayAtStart)
			{
				audio->Play();
			}
		}
	}

	void Scene::OnSimulationStop()
	{
		OGN_PROFILER_SCENE();

		m_Running = false;

		ScriptEngine::ClearSceneContext();
		m_Registry.view<NativeScriptComponent>().each([this](auto entity, auto &nsc)
		{
			nsc.DestroyScript(&nsc);
		});

		PhysicsEngine::OnSimulateStop(this);
		m_Physics2D->OnSimulationStop();

		// Audio
		auto view = m_Registry.view<AudioComponent>();
		for (auto &e : view)
		{
			auto &ac = view.get<AudioComponent>(e);

			if (const std::shared_ptr<AudioSource> &audio = AssetManager::GetAsset<AudioSource>(ac.Audio))
			{
				audio->Stop();
			}
		}
	}

	void Scene::RenderScene(const Camera &camera)
	{
		OGN_PROFILER_RENDERING();

		std::sort(m_EntityStorage.begin(), m_EntityStorage.end(), [&](const auto a, const auto b)
		{
			Entity eA { std::get<1>(a), this };
			Entity eB { std::get<1>(b), this };
			float aLen = eA.GetComponent<TransformComponent>().WorldTranslation.z;
			float bLen = eB.GetComponent<TransformComponent>().WorldTranslation.z;
			return aLen < bLen;
		});

        if (!camera.IsPerspective())
        {
            glDisable(GL_DEPTH_TEST);
        }
        else
        {
            glEnable(GL_DEPTH_TEST);
        }

		Renderer2D::Begin(camera);

		// Render All entities
		for (auto &e : m_EntityStorage)
		{
			Entity entity { e.second, this };
			auto &tc = entity.GetComponent<TransformComponent>();

			if (!tc.Visible)
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
							src.Min = anim->GetCurrentFrame().Min;
							src.Max = anim->GetCurrentFrame().Max;
						}
					}
				}
				Renderer2D::DrawSprite(tc.GetTransform(), src, static_cast<int>(e.second));
			}

			if (entity.HasComponent<CircleRendererComponent>())
			{
				auto &cc = entity.GetComponent<CircleRendererComponent>();
				Renderer2D::DrawCircle(tc.GetTransform(), cc.Color, cc.Thickness, cc.Fade,
															 static_cast<int>(e.second));
			}

			// Particles
			if (entity.HasComponent<ParticleComponent>())
			{
				auto &pc = entity.GetComponent<ParticleComponent>();
				for (int i = 0; i < 5; i++)
				{
					pc.Particle.Emit(pc,
						{ tc.WorldTranslation.x, tc.WorldTranslation.y, tc.WorldTranslation.z + pc.ZAxis },
						tc.WorldScale, pc.Rotation, static_cast<int>(e.second));
				}

				pc.Particle.OnRender();
			}

			// Text
			if (entity.HasComponent<TextComponent>())
			{
				auto &text = entity.GetComponent<TextComponent>();
				glm::mat4 transform = glm::mat4(1.0f);
				glm::mat4 invertedCamTransform = glm::mat4(1.0f);

				if (text.ScreenSpace)
				{
					if (camera.IsPerspective())
					{
						Entity primaryCam = GetPrimaryCameraEntity();
						if (primaryCam)
						{
							const auto &cc = primaryCam.GetComponent<CameraComponent>().Camera;
							const auto &ccTC = primaryCam.GetComponent<TransformComponent>();
							const float ratio = cc.GetAspectRatio();
							glm::vec2 scale = glm::vec2(tc.WorldScale.x, tc.WorldScale.y * ratio);
							transform = glm::translate(glm::mat4(1.0f), { tc.WorldTranslation.x, tc.WorldTranslation.y, 0.0f })
								* glm::toMat4(ccTC.Rotation)
								* glm::scale(glm::mat4(1.0f), { tc.WorldScale.x, tc.WorldScale.y * ratio, 0.0 });

							invertedCamTransform = glm::inverse(cc.GetViewProjection());
						}
					}
					else
					{
						const float ratio = camera.GetAspectRatio();
						transform = glm::scale(tc.GetTransform(), { tc.WorldScale.x, tc.WorldScale.y * ratio, 0.0 });
						invertedCamTransform = glm::inverse(camera.GetViewProjection());
					}

					transform = invertedCamTransform * transform;
				}
				else
				{
					transform = tc.GetTransform();
				}

				if (text.FontHandle != 0)
					Renderer2D::DrawString(text.TextString, transform, text, static_cast<int>(e.second));
			}
		}

		Renderer2D::End();

#if 0
		// ===============================
		// 3D Scene
		glEnable(GL_DEPTH_TEST);
		const auto &lightView = m_Registry.view<TransformComponent, LightComponent>();
		const auto &meshView = m_Registry.view<TransformComponent, StaticMeshComponent>();

        for (auto &li : lightView)
        {
            const auto &[lightTC, lc] = lightView.get<TransformComponent, LightComponent>(li);
			if(!lightTC.Visible)
				continue;
			MeshRenderer::Begin(camera);
            
            lc.Light->DirLightData.LightSpaceMat = lc.Light->GetShadow().ViewProjection;
            lc.Light->DirLightData.Position = glm::vec4(lightTC.WorldTranslation, 1.0f);
            lc.Light->LightingUBO->Bind();
            lc.Light->LightingUBO->SetData(&lc.Light->DirLightData, sizeof(lc.Light->DirLightData));
			MeshRenderer::AttachShadow(lc.Light->GetShadow().DepthMapID);

            for (auto e : meshView)
            {
                const auto [tc, mc] = meshView.get<TransformComponent, StaticMeshComponent>(e);
                if (!tc.Visible)
                    continue;

                std::shared_ptr<Material> material;
                if (mc.HMaterial) material = AssetManager::GetAsset<Material>(mc.HMaterial);
                else material = Renderer::GetMaterial("Mesh");
                switch (mc.mType)
                {
                case StaticMeshComponent::Type::Cube:
                    MeshRenderer::DrawCube(tc.GetTransform(), material.get(), (int)e);
                    break;
                case StaticMeshComponent::Type::Sphere:
                    MeshRenderer::DrawSphere(tc.GetTransform(), material.get(), (int)e);
                    break;
                case StaticMeshComponent::Type::Capsule:
                    MeshRenderer::DrawCapsule(tc.GetTransform(), material.get(), (int)e);
                    break;
                }
            }
			MeshRenderer::End();
        }
#endif
	}
	
	void Scene::RenderShadow(const glm::mat4 &viewProjection)
	{
		OGN_PROFILER_RENDERING();
        glEnable(GL_DEPTH_TEST);
		const auto &dirLight = m_Registry.view<TransformComponent, LightComponent>();
		for (auto &light : dirLight)
		{
			const auto &[lightTC, lc] = dirLight.get<TransformComponent, LightComponent>(light);
            if (!lightTC.Visible)
                continue;
		
	        float size = lc.Light->OrthoSize;  // Adjust based on your scene size
			glm::mat4 lightProjection = glm::ortho(-size, size, -size, size, lc.Light->NearPlane, lc.Light->FarPlane);
			glm::mat4 lightView = glm::lookAt(lightTC.WorldTranslation,
											  glm::eulerAngles(lightTC.Rotation),
                                              glm::vec3(0.0f, 1.0f, 0.0f));

            //Combine the projection and view matrices
            glm::mat4 lightViewProjection = lightProjection * lightView;
			lc.Light->GetShadow().ViewProjection = lightProjection * lightView;

			//lc.Light->GetShadow().CalculateViewProjection(lightTC, viewProjection);

            std::shared_ptr<Shader> &shader = lc.Light->GetShadow().GetDepthShader();
            shader->Enable();

            lc.Light->GetShadow().BindFBO();

            MeshRenderer::Begin(lc.Light->GetShadow().ViewProjection);
            shader->SetMatrix("viewProjection", lc.Light->GetShadow().ViewProjection);

			const auto &view = m_Registry.view<TransformComponent, StaticMeshComponent>();
			for (auto &e : view)
			{
				const auto &[tc, mc] = view.get<TransformComponent, StaticMeshComponent>(e);
				if (!tc.Visible)
				{
                    continue;
				}

                switch (mc.mType)
                {
                case StaticMeshComponent::Type::Cube:
					MeshRenderer::DrawCube(tc.GetTransform(), {1.0f, 1.0f, 1.0f, 1.0f});
                    break;
                case StaticMeshComponent::Type::Sphere:
                    MeshRenderer::DrawSphere(tc.GetTransform(), {1.0f, 1.0f, 1.0f, 1.0f});
                    break;
                case StaticMeshComponent::Type::Capsule:
                    MeshRenderer::DrawCapsule(tc.GetTransform(), {1.0f, 1.0f, 1.0f, 1.0f});
                    break;
                }
			}
			
			MeshRenderer::End();
			lc.Light->GetShadow().UnbindFBO();
			shader->Disable();
		}
	}

    void Scene::UpdateTransform()
    {
        OGN_PROFILER_FUNCTION();

        const auto &view = m_Registry.view<IDComponent, TransformComponent>();
        for (auto entity : view)
        {
            const auto &[idc, tc] = view.get<IDComponent, TransformComponent>(entity);
			if (idc.Parent)
			{
				auto parent = GetEntityWithUUID(idc.Parent);
				if (parent)
				{
                    auto &ptc = parent.GetComponent<TransformComponent>();
                    glm::vec3 rotatedLocalPos = ptc.WorldRotation * tc.Translation;
                    tc.WorldTranslation = rotatedLocalPos + ptc.WorldTranslation;
                    tc.WorldRotation = ptc.WorldRotation * tc.Rotation;
                    tc.WorldScale = tc.Scale;
				}
			}
			else
			{
                tc.WorldTranslation = tc.Translation;
                tc.WorldRotation = tc.Rotation;
                tc.WorldScale = tc.Scale;
			}
        }
    }

    void Scene::SetFocus(bool focus)
    {
		m_IsFocus = focus;
    }

    void Scene::LockMouse()
    {
		m_IsFocus = true;
		Input::Get().MouseHide();
    }

    void Scene::UnlockMouse()
    {
        m_IsFocus = false;
        Input::Get().MouseUnHide();
    }

    void Scene::OnViewportResize(const uint32_t width, const uint32_t height)
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

	void Scene::Step(int frames)
	{
		m_StepFrames = frames;
	}

    template <typename T>
	void Scene::OnComponentAdded(Entity entity, T &component)
	{ }

#define OGN_ADD_COMPONENT(components)\
	template<>\
	void Scene::OnComponentAdded<components>(Entity entity, components &component){}

	OGN_ADD_COMPONENT(IDComponent)
	OGN_ADD_COMPONENT(TagComponent)
	OGN_ADD_COMPONENT(TransformComponent)
	OGN_ADD_COMPONENT(UIComponent)
	OGN_ADD_COMPONENT(AudioComponent)
	OGN_ADD_COMPONENT(AudioListenerComponent)
	OGN_ADD_COMPONENT(SpriteAnimationComponent)
	OGN_ADD_COMPONENT(SpriteRenderer2DComponent)
	OGN_ADD_COMPONENT(StaticMeshComponent)
	OGN_ADD_COMPONENT(ModelComponent)
	OGN_ADD_COMPONENT(TextComponent)
	OGN_ADD_COMPONENT(CircleRendererComponent)
	OGN_ADD_COMPONENT(NativeScriptComponent)
	OGN_ADD_COMPONENT(ScriptComponent)
	OGN_ADD_COMPONENT(ParticleComponent)
	// 2D Physics
	OGN_ADD_COMPONENT(Rigidbody2DComponent)
	OGN_ADD_COMPONENT(BoxCollider2DComponent)
	OGN_ADD_COMPONENT(CircleCollider2DComponent)
	OGN_ADD_COMPONENT(RevoluteJoint2DComponent)
	// 3D Physics
	OGN_ADD_COMPONENT(RigidbodyComponent)

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
        if (!entity.HasComponent<RigidbodyComponent>())
        {
            entity.AddComponent<RigidbodyComponent>();
        }
	}

    template<>
    void Scene::OnComponentAdded(Entity entity, SphereColliderComponent &component)
    {
		if (!entity.HasComponent<RigidbodyComponent>())
		{
			entity.AddComponent<RigidbodyComponent>();
		}
    }

    template<>
    void Scene::OnComponentAdded(Entity entity, CapsuleColliderComponent &component)
    {
        if (!entity.HasComponent<RigidbodyComponent>())
        {
            entity.AddComponent<RigidbodyComponent>();
        }
    }

	template<>
    void Scene::OnComponentAdded(Entity entity, LightComponent &component)
    {
		if (!component.Light)
		{
			component.Light = Lighting::Create(LightingType::Directional);
		}
    }
}

