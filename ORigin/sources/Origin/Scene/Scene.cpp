// Copyright (c) Evangelion Manuhutu | ORigin Engine

#include "pch.h"
#include "Entity.h"
#include "Scene.h"
#include "Origin/Audio/Audio.h"
#include "ScriptableEntity.h"
#include "Lighting.h"

#include "Origin\Asset\AssetManager.h"
#include "Origin/Animation/Animation.h"
#include "Origin/Scripting/ScriptEngine.h"
#include "Origin/Renderer/Renderer.h"
#include "Origin/Renderer/Renderer2D.h"
#include "Origin/Renderer/Renderer3D.h"
#include "origin/Physics/Contact2DListener.h"
#include "Origin/Physics/Physics2D.h"

#include <glm/glm.hpp>

namespace origin
{
	class BoxColliderComponent;
	class RigidbodyComponent;
	class SphereColliderComponent;

	Scene::Scene()
	{
		if (!m_PhysicsScene)
			m_PhysicsScene = PhysicsScene::Create(this);

		m_Physics2D = new Physics2D(this);
	}

	Scene::~Scene()
	{
		delete m_Physics2D;
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

				const auto& srcComponent = src.get<Component>(srcEntity);
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
		auto newScene = std::make_shared<Scene>();

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

			enttMap[uuid] = static_cast<entt::entity>(newEntity);
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
		entity.AddComponent<LightComponent>();

		auto& tag = entity.AddComponent<TagComponent>();
		tag.Tag = "Point Light";

		UUID& uuid = entity.GetComponent<IDComponent>().ID;
		entity.GetComponent<TransformComponent>().Translation.y = 5.0f;

		entity.GetComponent<LightComponent>().Light = Lighting::Create(LightingType::Point);

		m_EntityMap.insert(std::make_pair(uuid, entity));

		return entity;
	}

	Entity Scene::CreateSpotLight()
	{
		Entity entity = {m_Registry.create(), this};
		entity.AddComponent<IDComponent>(UUID());
		entity.AddComponent<TransformComponent>();
		entity.AddComponent<LightComponent>();

		auto& tag = entity.AddComponent<TagComponent>();
		tag.Tag = "Spot Light";

		UUID& uuid = entity.GetComponent<IDComponent>().ID;
		entity.GetComponent<TransformComponent>().Translation.y = 3.0f;

		entity.GetComponent<LightComponent>().Light = Lighting::Create(LightingType::Spot);

		m_EntityMap.insert(std::make_pair(uuid, entity));

		return entity;
	}

	Entity Scene::CreateDirectionalLight()
	{
		Entity entity = {m_Registry.create(), this};
		entity.AddComponent<IDComponent>(UUID());
		entity.AddComponent<TransformComponent>();
		entity.AddComponent<LightComponent>();

		auto& tag = entity.AddComponent<TagComponent>();
		tag.Tag = "Directional Light";

		UUID& uuid = entity.GetComponent<IDComponent>().ID;
		entity.GetComponent<TransformComponent>().Rotation.x = glm::radians(-90.0f);

		entity.GetComponent<LightComponent>().Light = Lighting::Create(LightingType::Directional);

		FramebufferSpecification fbSpec;
		fbSpec.Width = 1080;
		fbSpec.Height = 1080;

		fbSpec.WrapMode = GL_CLAMP_TO_BORDER;
		fbSpec.ReadBuffer = false;

		fbSpec.Attachments = {FramebufferTextureFormat::DEPTH};
		//entity.GetComponent<LightComponent>().ShadowFb = Framebuffer::Create(fbSpec);

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

			// Physics
			m_PhysicsScene->Simulate(deltaTime);
			m_Physics2D->Simulate(deltaTime);
			
		}

		// Rendering
		const auto& cameraView = m_Registry.view<CameraComponent, TransformComponent>();
		for (const auto entity : cameraView)
		{
			auto& [tc, camera] = cameraView.get<TransformComponent, CameraComponent>(entity);

			if (camera.Primary)
			{
				Renderer::BeginScene(camera.Camera, tc);
				RenderScene(camera.Camera, tc);
				Renderer::EndScene();
				break;
			}
		}

		// Particle Update
		m_Registry.view<ParticleComponent>().each([=](auto entity, auto& pc)
		{
			pc.Particle.OnUpdate(deltaTime);
		});

		// Animation
		const auto& animView = m_Registry.view<AnimationComponent>();
		for (const auto entity : animView)
		{
			auto& ac = animView.get<AnimationComponent>(entity);
			if (ac.State.HasAnimations())
				ac.State.GetAnimation().Update(deltaTime);
		}

		glm::vec3 camVel(0.0f), camPos(0.0f);
		const auto& camListenerView = m_Registry.view<CameraComponent, AudioListener>();
		for(const auto entity : camListenerView)
		{
			auto& listener = camListenerView.get<AudioListener>(entity);
			camPos = listener.GetPosition();
			camPos = listener.GetVelocity();
		}

		// Audio Update
		const auto& audioView = m_Registry.view<TransformComponent, AudioComponent>();
		for (const auto entity : audioView)
		{
			auto& [tc, ac] = audioView.get<TransformComponent, AudioComponent>(entity);
			
			if (const std::shared_ptr<Audio>& audio = AssetManager::GetAsset<Audio>(ac.Audio))
			{
				audio->UpdateAudioComponent(ac);
				if (ac.Spatial)
				{
					static glm::vec3 prevPos = tc.Translation;

					const glm::vec3& position = tc.Translation;
					const glm::vec3 delta = position - prevPos;

					prevPos = position;

					glm::vec3 velocity = (position - prevPos) / glm::vec3(deltaTime);
					audio->Set3DAttributes(tc.Translation, velocity);
					audio->UpdateDopplerEffect(camPos, camVel);
				}
			}
		}

		const auto& audioListenerView = m_Registry.view<TransformComponent, AudioListenerComponent>();
		for (const auto entity : audioListenerView)
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
			m_Registry.view<ParticleComponent>().each([=](auto entity, auto& pc)
			{
				pc.Particle.OnUpdate(deltaTime);
			});
		}

		// Animation
		const auto& animView = m_Registry.view<AnimationComponent>();
		for (const auto entity : animView)
		{
			auto& ac = animView.get<AnimationComponent>(entity);
			if (ac.State.HasAnimations())
			{
				if (ac.State.Preview)
					ac.State.Update(deltaTime);
				else
					ac.State.GetAnimation().Reset();
			}
		}

		// Audio Update
		Renderer2D::BeginScene();
		const auto& audioView = m_Registry.view<TransformComponent, AudioComponent>();
		for (auto entity : audioView)
		{
			auto& [tc, ac] = audioView.get<TransformComponent, AudioComponent>(entity);

			if (const std::shared_ptr<Audio>& audio = AssetManager::GetAsset<Audio>(ac.Audio))
			{
				audio->UpdateAudioComponent(ac);
				if (ac.Spatial)
				{
					static glm::vec3 prevPos = tc.Translation;

					const glm::vec3& position = tc.Translation;
					const glm::vec3 delta = position - prevPos;

					prevPos = position;

					glm::vec3 velocity = delta / glm::vec3(deltaTime);
					audio->Set3DAttributes(tc.Translation, velocity);
					audio->UpdateDopplerEffect(editorCamera.GetAudioListener().GetPosition(), editorCamera.GetAudioListener().GetVelocity());
					
				}
			}
		}
		Renderer2D::EndScene();

		editorCamera.UpdateAudioListener(deltaTime);
		AudioEngine::SetMute(false);
		AudioEngine::SystemUpdate();
	}

	void Scene::OnUpdateSimulation(Timestep deltaTime, EditorCamera& editorCamera)
	{
		if (!m_Paused || m_StepFrames-- > 0)
		{
			// Update Scripts
			{
				const auto& view = m_Registry.view<ScriptComponent>();
				for (const auto e : view)
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

			//Physics
			m_PhysicsScene->Simulate(deltaTime);
			m_Physics2D->Simulate(deltaTime);
		}

		// Render
		RenderScene(editorCamera);

		{
			m_Registry.view<ParticleComponent>().each([=](auto entity, auto& pc)
			{
				pc.Particle.OnUpdate(deltaTime);
			});
		}

		// Animation
		const auto& animView = m_Registry.view<AnimationComponent>();
		for (const auto entity : animView)
		{
			auto& ac = animView.get<AnimationComponent>(entity);
			if (ac.State.HasAnimations())
				ac.State.GetAnimation().Update(deltaTime);
		}

		// Audio Update
		Renderer2D::BeginScene();
		AudioEngine::SetMute(false);
		const auto& audioView = m_Registry.view<TransformComponent, AudioComponent>();
		for (auto entity : audioView)
		{
			auto& [tc, ac] = audioView.get<TransformComponent, AudioComponent>(entity);
			
			if (const std::shared_ptr<Audio>& audio = AssetManager::GetAsset<Audio>(ac.Audio))
			{
				audio->UpdateAudioComponent(ac);

				if (ac.Spatial)
				{
					static glm::vec3 prevPos = tc.Translation;

					const glm::vec3& position = tc.Translation;
					const glm::vec3 delta = position - prevPos;

					prevPos = position;

					glm::vec3 velocity = (position - prevPos) / glm::vec3(deltaTime);
					audio->Set3DAttributes(tc.Translation, velocity);
					
					audio->UpdateDopplerEffect(editorCamera.GetAudioListener().GetPosition(), editorCamera.GetAudioListener().GetVelocity());
				}
			}
		}
		Renderer2D::EndScene();

		bool isMainCameraListening = false;
		const auto& audioListenerView = m_Registry.view<TransformComponent, AudioListenerComponent>();
		for (const auto entity : audioListenerView)
		{
			auto& [tc, al] = audioListenerView.get<TransformComponent, AudioListenerComponent>(entity);
			if (al.Enable)
			{
				static glm::vec3 prevPos = tc.Translation;
				const glm::vec3& position = tc.Translation;
				const glm::vec3 delta = position - prevPos;
				prevPos = position;
				glm::vec3 velocity = (position - prevPos) / glm::vec3(deltaTime);

				al.Listener.Set(tc.Translation, velocity, tc.GetForward(), tc.GetUp());
			}

			isMainCameraListening = al.Enable;
		}

		if (!isMainCameraListening)
			editorCamera.UpdateAudioListener(deltaTime);

		AudioEngine::SystemUpdate();
	}

	void Scene::OnSimulationStart()
	{
		m_Running = true;


		ScriptEngine::SetSceneContext(this);
		const auto& scriptView = m_Registry.view<ScriptComponent>();
		for (const auto e : scriptView)
		{
			Entity entity = { e, this };
			ScriptEngine::OnCreateEntity(entity);
		}

		m_PhysicsScene->OnSimulationStart();
		m_Physics2D->OnSimulationStart();

		// Audio
		const auto& audioView = m_Registry.view<AudioComponent>();
		for (auto& e : audioView)
		{
			auto& ac = audioView.get<AudioComponent>(e);
			std::shared_ptr<Audio>& audio = AssetManager::GetAsset<Audio>(ac.Audio);
			if (ac.PlayAtStart)
			{
				audio->SetGain(ac.Volume);
				audio->Play();
			}
		}
	}

	void Scene::OnSimulationStop()
	{
		m_Running = false;

		ScriptEngine::ClearSceneContext();

		m_PhysicsScene->OnSimulationStop();
		m_Physics2D->OnSimulationStop();

		// Audio
		auto view = m_Registry.view<AudioComponent>();
		for (auto& e : view)
		{
			auto& ac = view.get<AudioComponent>(e);
			
			if (const std::shared_ptr<Audio>& audio = AssetManager::GetAsset<Audio>(ac.Audio))
				audio->Stop();
		}
	}

	void Scene::RenderScene(const SceneCamera& camera, const TransformComponent& cameraTransform)
	{
		const auto& lightView = m_Registry.view<TransformComponent, LightComponent>();

		// Particle
		{
			auto& view = m_Registry.view<TransformComponent, ParticleComponent>();
			for (auto entity : view)
			{
				auto& [tc, pc] = view.get<TransformComponent, ParticleComponent>(entity);

				for (int i = 0; i < 5; i++)
				{
					pc.Particle.Emit(
						pc,
						glm::vec3(tc.Translation.x, tc.Translation.y, tc.Translation.z + pc.ZAxis),
						tc.Scale, tc.Rotation, static_cast<int>(entity)
					);
				}
				pc.Particle.OnRender();
			}
		}

		// Sprites
		{
			auto& view = m_Registry.view<TransformComponent, SpriteRenderer2DComponent>();
			std::vector<entt::entity> entities(view.begin(), view.end());

			std::sort(entities.begin(), entities.end(),
				[=](const entt::entity& a, const entt::entity& b) {
					const auto& objA = m_Registry.get<TransformComponent>(a);
					const auto& objB = m_Registry.get<TransformComponent>(b);
					return glm::length(camera.GetPosition().z - objA.Translation.z) > glm::length(camera.GetPosition().z - objB.Translation.z);
				});
			for (const entt::entity& entity : entities)
			{
				auto& [transform, sprite] = view.get<TransformComponent, SpriteRenderer2DComponent>(entity);
				Renderer2D::DrawSprite(transform.GetTransform(), sprite, static_cast<int>(entity));
			}

			const auto& animView = m_Registry.view<SpriteRenderer2DComponent, AnimationComponent>();
			for (auto& entity : animView)
			{
				auto& [sprite, anim] = animView.get<SpriteRenderer2DComponent, AnimationComponent>(entity);
				if (anim.State.HasAnimations() == false)
					continue;

				if (anim.State.GetAnimation().HasFrame())
					sprite.Texture = anim.State.GetAnimation().GetCurrentValue();
			}
		}

		// Circles
		{
			auto& view = m_Registry.view<TransformComponent, CircleRendererComponent>();
			std::vector<entt::entity> entities(view.begin(), view.end());

			std::sort(entities.begin(), entities.end(),
				[=](const entt::entity& a, const entt::entity& b) {
					const auto& objA = m_Registry.get<TransformComponent>(a);
					const auto& objB = m_Registry.get<TransformComponent>(b);
					return glm::length(camera.GetPosition().z - objA.Translation.z) > glm::length(camera.GetPosition().z - objB.Translation.z);
				});
			for (auto& entity : entities)
			{
				auto [transform, circle] = view.get<TransformComponent, CircleRendererComponent>(entity);
				Renderer2D::DrawCircle(transform.GetTransform(), circle.Color, circle.Thickness, circle.Fade,
				                       static_cast<int>(entity));
			}
		}

		// Text
		const auto& textView = m_Registry.view<TransformComponent, TextComponent>();
		for (const auto entity : textView)
		{
			auto [transform, text] = textView.get<TransformComponent, TextComponent>(entity);
			Renderer2D::DrawString(text.TextString, transform.GetTransform(), text, static_cast<int>(entity));
		}

		glEnable(GL_CULL_FACE);
		glCullFace(GL_BACK);

		// 3D Scene
		const auto& spriteView = m_Registry.view<TransformComponent, SpriteRendererComponent>();
		for (const auto entity : spriteView)
		{
			auto& [transform, sprite] = spriteView.get<TransformComponent, SpriteRendererComponent>(entity);
			Renderer3D::DrawCube(transform.GetTransform(), sprite, static_cast<int>(entity));
		}

		// Mesh
		{
			const auto& meshView = m_Registry.view<TransformComponent, StaticMeshComponent>();
			for (auto& entity : meshView)
			{
				auto& [tc, mesh] = meshView.get<TransformComponent, StaticMeshComponent>(entity);

				Lighting::PointLightCount = 0;
				Lighting::SpotLightCount = 0;

				if (mesh.Model)
				{
					mesh.Material->EnableShader();
					mesh.Material->SetFloat("material.Bias", mesh.Material->Bias);

					for (auto& light : lightView)
					{
						auto& [tc, lc] = lightView.get<TransformComponent, LightComponent>(light);
						lc.Light->OnUpdate(tc, mesh.Material);
					}

					mesh.Model->Draw(tc.GetTransform(), camera, cameraTransform.GetTransform(), static_cast<int>(entity));
					mesh.Material->DisableShader();

					if (mesh.Material->m_PointLightCount != Lighting::PointLightCount ||
						mesh.Material->m_SpotLightCount != Lighting::SpotLightCount)
						mesh.Material->RefreshShader();

					mesh.Material->m_PointLightCount = Lighting::PointLightCount;
					mesh.Material->m_SpotLightCount = Lighting::SpotLightCount;
				}
			}
		}
		

		glDisable(GL_CULL_FACE);
	}

	void Scene::RenderScene(const EditorCamera& camera)
	{
		Renderer::BeginScene(camera);

		//m_Skybox->Draw(camera);

		
		const auto& lightView = m_Registry.view<TransformComponent, LightComponent>();
		// Particle
		{
			auto& view = m_Registry.view<TransformComponent, ParticleComponent>();
			for (auto entity : view)
			{
				auto& [tc, pc] = view.get<TransformComponent, ParticleComponent>(entity);

				for (int i = 0; i < 5; i++)
				{
					pc.Particle.Emit(pc,
					                 glm::vec3(tc.Translation.x, tc.Translation.y, tc.Translation.z + pc.ZAxis),
					                 tc.Scale, pc.Rotation, static_cast<int>(entity)
					);
				}

				pc.Particle.OnRender();
			}
		}

		// Sprites
		{
			auto& view = m_Registry.view<TransformComponent, SpriteRenderer2DComponent>();
			std::vector<entt::entity> entities(view.begin(), view.end());

			std::sort(entities.begin(), entities.end(), [=](const entt::entity& a, const entt::entity& b)
				{
					const auto& objA = m_Registry.get<TransformComponent>(a);
					const auto& objB = m_Registry.get<TransformComponent>(b);
					return glm::length(camera.GetPosition().z - objA.Translation.z) > glm::length(camera.GetPosition().z - objB.Translation.z);
				});

			
			for (const entt::entity& entity : entities)
			{
				auto& [transform, sprite] = view.get<TransformComponent, SpriteRenderer2DComponent>(entity);

				for (auto& light : lightView)
				{
					auto& [tc, lc] = lightView.get<TransformComponent, LightComponent>(light);
					lc.Light->OnUpdate(tc);
				}

				Renderer2D::DrawSprite(transform.GetTransform(), sprite, static_cast<int>(entity));
			}

			const auto& animView = m_Registry.view<SpriteRenderer2DComponent, AnimationComponent>();
			for (auto& entity : animView)
			{
				auto& [sprite, anim] = animView.get<SpriteRenderer2DComponent, AnimationComponent>(entity);
				if (anim.State.HasAnimations() == false)
					continue;

				if (anim.State.GetAnimation().HasFrame())
					sprite.Texture = anim.State.GetAnimation().GetCurrentValue();
			}
		}

		// Circles
		{
			auto& view = m_Registry.view<TransformComponent, CircleRendererComponent>();
			std::vector<entt::entity> entities(view.begin(), view.end());

			std::sort(entities.begin(), entities.end(), [=](const entt::entity& a, const entt::entity& b)
				{
					const auto& objA = m_Registry.get<TransformComponent>(a);
					const auto& objB = m_Registry.get<TransformComponent>(b);
					return glm::length(camera.GetPosition().z - objA.Translation.z) > glm::length(camera.GetPosition().z - objB.Translation.z);
				});

			for (auto& entity : entities)
			{
				auto& [transform, circle] = view.get<TransformComponent, CircleRendererComponent>(entity);
				for (auto& light : lightView)
				{
					auto& [tc, lc] = lightView.get<TransformComponent, LightComponent>(light);
					lc.Light->OnUpdate(tc);
				}
				Renderer2D::DrawCircle(transform.GetTransform(), circle.Color, circle.Thickness, circle.Fade,
				                       static_cast<int>(entity));
			}
		}

		// Text
		{
			const auto& view = m_Registry.view<TransformComponent, TextComponent>();
			for (auto entity : view)
			{
				auto [transform, text] = view.get<TransformComponent, TextComponent>(entity);
				for (auto& light : lightView)
				{
					auto& [tc, lc] = lightView.get<TransformComponent, LightComponent>(light);
					lc.Light->OnUpdate(tc);
				}
				Renderer2D::DrawString(text.TextString, transform.GetTransform(), text, static_cast<int>(entity));
			}
		}

		glEnable(GL_CULL_FACE);
		glCullFace(GL_BACK);
		// 3D Scene
		// Cube
		const auto& cubeView = m_Registry.view<TransformComponent, SpriteRendererComponent>();
		std::vector<entt::entity> cubeEntities(cubeView.begin(), cubeView.end());
		std::sort(cubeEntities.begin(), cubeEntities.end(),
		          [=](const entt::entity& a, const entt::entity& b)
		          {
			          const auto& objA = m_Registry.get<TransformComponent>(a);
			          const auto& objB = m_Registry.get<TransformComponent>(b);
			          return length(camera.GetPosition() - objA.Translation) > length(
				          camera.GetPosition() - objB.Translation);
		          });

		for (const entt::entity cube : cubeEntities)
		{
			auto& [transform, sprite] = cubeView.get<TransformComponent, SpriteRendererComponent>(cube);
			Renderer3D::DrawCube(transform.GetTransform(), sprite, static_cast<int>(cube));
		}

		// Mesh
		const auto& meshView = m_Registry.view<TransformComponent, StaticMeshComponent>();
		for (auto& entity : meshView)
		{
			auto& [tc, mesh] = meshView.get<TransformComponent, StaticMeshComponent>(entity);
			
			Lighting::PointLightCount = 0;
			Lighting::SpotLightCount = 0;

			if (mesh.Model)
			{
				mesh.Material->EnableShader();
				mesh.Material->SetFloat("material.Bias", mesh.Material->Bias);

				for (auto& light : lightView)
				{
					auto& [tc, lc] = lightView.get<TransformComponent, LightComponent>(light);
					lc.Light->OnUpdate(tc, mesh.Material);
				}

				mesh.Model->Draw(tc.GetTransform(), camera, static_cast<int>(entity));
				mesh.Material->DisableShader();

				if (mesh.Material->m_PointLightCount != Lighting::PointLightCount || 
					mesh.Material->m_SpotLightCount != Lighting::SpotLightCount)
					mesh.Material->RefreshShader();

				mesh.Material->m_PointLightCount = Lighting::PointLightCount;
				mesh.Material->m_SpotLightCount = Lighting::SpotLightCount;
			}
		}
		glDisable(GL_CULL_FACE);

		//DrawGrid(m_GridSize, m_GridColor);
		Renderer::EndScene();
	}

	void Scene::OnShadowRender()
	{
		// ==============================
		// Directional Light Shadow
		// ==============================
		Renderer::GetGShader("DirLightDepthMap")->Enable();

		const auto& dirLight = m_Registry.view<TransformComponent, LightComponent>();
		for (auto& light : dirLight)
		{
			auto& [tc, lc] = dirLight.get<TransformComponent, LightComponent>(light);
			lc.Light->SetupShadow(tc);

			// Render
			const auto& meshView = m_Registry.view<TransformComponent, StaticMeshComponent>();
			for (auto& entity : meshView)
			{
				auto& [tc, mesh] = meshView.get<TransformComponent, StaticMeshComponent>(entity);

				if (mesh.Model)
				{
					// Set Depth shader
					Renderer::GetGShader("DirLightDepthMap")->SetMatrix("uModel", tc.GetTransform());
					mesh.Model->Draw();
				}
			}

			lc.Light->GetShadow()->GetFramebuffer()->Unbind();
		}

		Renderer::GetGShader("DirLightDepthMap")->Disable();

		glDisable(GL_CULL_FACE);
	}

	void Scene::OnRuntimeStart()
	{
		m_Running = true;

		ScriptEngine::SetSceneContext(this);
		auto scriptView = m_Registry.view<ScriptComponent>();
		for (auto e : scriptView)
		{
			Entity entity = { e, this };
			ScriptEngine::OnCreateEntity(entity);
		}

		auto audioView = m_Registry.view<AudioComponent>();
		for (auto& e : audioView)
		{
			auto& ac = audioView.get<AudioComponent>(e);
			std::shared_ptr<Audio>& audio = AssetManager::GetAsset<Audio>(ac.Audio);

			if (ac.PlayAtStart)
			{
				audio->SetGain(ac.Volume);
				audio->Play();
			}
		}

		m_PhysicsScene->OnSimulationStart();
		m_Physics2D->OnSimulationStart();
	}

	void Scene::OnRuntimeStop()
	{
		m_Running = false;

		ScriptEngine::ClearSceneContext();

		const auto& audioView = m_Registry.view<AudioComponent>();
		for (auto& e : audioView)
		{
			auto& ac = audioView.get<AudioComponent>(e);
			if (const std::shared_ptr<Audio>& audio = AssetManager::GetAsset<Audio>(ac.Audio))
				audio->Stop();
		}

		m_PhysicsScene->OnSimulationStop();
		m_Physics2D->OnSimulationStop();
	}

	void Scene::OnViewportResize(const uint32_t width, const uint32_t height)
	{
		if (m_ViewportHeight == height && m_ViewportWidth == width)
			return;

		m_ViewportWidth = width;
		m_ViewportHeight = height;

		const auto& view = m_Registry.view<CameraComponent>();
		for (auto& e : view)
		{
			auto& cc = view.get<CameraComponent>(e);
			if (cc.Camera.GetAspectRatioType() == SceneCamera::AspectRatioType::Free)
				cc.Camera.SetViewportSize(width, height);
		}
	}

	Entity Scene::DuplicateEntity(Entity entity)
	{
		std::string name = entity.GetTag();
		Entity newEntity = CreateEntity(name);

		CopyComponentIfExists(AllComponents{}, newEntity, entity);

		return newEntity;
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
		const auto& view = m_Registry.view<CameraComponent>();
		for (auto& entity : view)
		{
			const CameraComponent& camera = view.get<CameraComponent>(entity);
			if (camera.Primary)
				return Entity{entity, this};
		}
		return {};
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
	OGN_REG_COMPONENT(LightComponent)
	OGN_REG_COMPONENT(StaticMeshComponent)
	OGN_REG_COMPONENT(TextComponent)
	OGN_REG_COMPONENT(CircleRendererComponent)
	OGN_REG_COMPONENT(TagComponent)
	OGN_REG_COMPONENT(NativeScriptComponent)
	OGN_REG_COMPONENT(ScriptComponent)
	OGN_REG_COMPONENT(Rigidbody2DComponent)
	OGN_REG_COMPONENT(BoxCollider2DComponent)
	OGN_REG_COMPONENT(CircleCollider2DComponent)
	OGN_REG_COMPONENT(ParticleComponent)
	OGN_REG_COMPONENT(RigidbodyComponent)
	OGN_REG_COMPONENT(BoxColliderComponent)
	OGN_REG_COMPONENT(SphereColliderComponent)

	template <>
	void Scene::OnComponentAdded<CameraComponent>(Entity entity, CameraComponent& component)
	{
		if (m_ViewportWidth > 0 && m_ViewportHeight > 0)
			component.Camera.SetViewportSize(m_ViewportWidth, m_ViewportHeight);
	}
}
