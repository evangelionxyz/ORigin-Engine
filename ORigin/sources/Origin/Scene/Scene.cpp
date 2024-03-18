// Copyright (c) Evangelion Manuhutu | ORigin Engine

#include "pch.h"
#include "Scene.h"
#include "Entity.h"
#include "Lighting.h"
#include "EntityManager.h"
#include "ScriptableEntity.h"
#include "Origin/Audio/AudioEngine.h"
#include "Origin/Audio/AudioSource.h"
#include "Origin/Animation/Animation.h"
#include "origin/Physics/Contact2DListener.h"
#include "Origin/Physics/Physics2D.h"
#include "Origin/Renderer/Renderer.h"
#include "Origin/Renderer/Renderer2D.h"
#include "Origin/Renderer/Renderer3D.h"
#include "Origin/Scripting/ScriptEngine.h"
#include "Origin/Asset/AssetManager.h"
#include <glm/glm.hpp>

#pragma warning(disable : OGN_DISABLED_WARNINGS)

namespace origin
{
	class BoxColliderComponent;
	class RigidbodyComponent;
	class SphereColliderComponent;
	class CapsuleColliderComponent;

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
			newEntity = EntityManager::CreateEntityWithUUID(uuid, name, newScene.get());
			enttMap[uuid] = static_cast<entt::entity>(newEntity);
		}

		// Copy components (except IDComponent and TagComponent) into new scene (destination)
		CopyComponent(AllComponents{}, dstSceneRegistry, srcSceneRegistry, enttMap);

		return newScene;
	}

	Entity Scene::GetEntityWithUUID(UUID uuid)
	{
		if (m_EntityMap.find(uuid) != m_EntityMap.end())
			return { m_EntityMap.at(uuid), this };
		return {};
	}

	Entity Scene::FindEntityByName(std::string_view name)
	{
		auto view = m_Registry.view<TagComponent>();
		for (auto entity : view)
		{
			const TagComponent& tc = view.get<TagComponent>(entity);
			if (tc.Tag == name)
				return Entity{ entity, this };
		}

		return {};
	}

	Entity Scene::GetPrimaryCameraEntity()
	{
		const auto &view = m_Registry.view<CameraComponent>();
		for (auto &entity : view)
		{
			const CameraComponent &camera = view.get<CameraComponent>(entity);
			if (camera.Primary)
				return Entity { entity, this };
		}
		return {};
	}

	void Scene::OnUpdateRuntime(Timestep ts)
	{
		if (!m_Paused || m_StepFrames-- > 0)
		{
			// Update Scripts
			auto& scriptView = m_Registry.view<ScriptComponent>();
			for (auto e : scriptView)
			{
				Entity entity = {e, this};
				ScriptEngine::OnUpdateEntity(entity, (float)ts);
			}

			m_Registry.view<NativeScriptComponent>().each([=](auto entity, auto& nsc)
			{
				if (!nsc.Instance)
				{
					nsc.Instance = nsc.InstantiateScript();
					nsc.Instance->m_Entity = Entity{entity, this};
					nsc.Instance->OnCreate();
				}
				nsc.Instance->OnUpdate(ts);
			});

			// Particle Update
			m_Registry.view<ParticleComponent>().each([=](auto entity, auto& pc)
				{
					pc.Particle.OnUpdate(ts);
				});


			// Animation
			const auto& animView = m_Registry.view<AnimationComponent>();
			for (const auto entity : animView)
			{
				auto& ac = animView.get<AnimationComponent>(entity);
				if (!ac.Animations.empty())
					ac.Animations[ac.CurrentAnimation]->OnUpdateRuntime();
			}

			// Audio Update
			const auto& audioView = m_Registry.view<TransformComponent, AudioComponent>();
			for (const auto entity : audioView)
			{
				auto& [tc, ac] = audioView.get<TransformComponent, AudioComponent>(entity);

				if (std::shared_ptr<AudioSource>& audio = AssetManager::GetAsset<AudioSource>(ac.Audio))
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

			const auto& audioListenerView = m_Registry.view<TransformComponent, AudioListenerComponent>();
			for (const auto entity : audioListenerView)
			{
				auto& [tc, al] = audioListenerView.get<TransformComponent, AudioListenerComponent>(entity);
				
			}

			m_PhysicsScene->Simulate(ts);
			m_Physics2D->Simulate(ts);
		}
		

		// Rendering
		auto& cameraView = m_Registry.view<CameraComponent, TransformComponent>();
		for (auto entity : cameraView)
		{
			auto& [tc, camera] = cameraView.get<TransformComponent, CameraComponent>(entity);

			if (camera.Primary)
			{
				RenderScene(camera.Camera, tc);
				break;
			}
		}
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
			std::shared_ptr<AudioSource>& audio = AssetManager::GetAsset<AudioSource>(ac.Audio);
			if (ac.PlayAtStart)
				audio->Play();
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
			if (const std::shared_ptr<AudioSource>& audio = AssetManager::GetAsset<AudioSource>(ac.Audio))
				audio->Stop();
		}

		m_PhysicsScene->OnSimulationStop();
		m_Physics2D->OnSimulationStop();
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
			if (!ac.Animations.empty())
			{
				ac.Animations[ac.CurrentAnimation]->OnUpdateEditor();
			}
		}

		// Audio Update
		Renderer2D::Begin(editorCamera);
		const auto& audioView = m_Registry.view<TransformComponent, AudioComponent>();
		for (auto entity : audioView)
		{
			auto& [tc, ac] = audioView.get<TransformComponent, AudioComponent>(entity);
			if (std::shared_ptr<AudioSource> &audio = AssetManager::GetAsset<AudioSource>(ac.Audio))
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

		editorCamera.UpdateAudioListener(deltaTime);
	}

	void Scene::OnUpdateSimulation(Timestep ts, EditorCamera& editorCamera)
	{
		if (!m_Paused || m_StepFrames-- > 0)
		{
			// Update Scripts
			{
				m_Registry.view<ScriptComponent>().each([=](auto entityID, auto& sc)
				{
					Entity entity{ entityID, this };
					ScriptEngine::OnUpdateEntity(entity, (float)ts);
				});

				m_Registry.view<NativeScriptComponent>().each([=](auto entityID, auto& nsc)
				{
					if (!nsc.Instance)
					{
						nsc.Instance = nsc.InstantiateScript();
						nsc.Instance->m_Entity = Entity{ entityID, this};
						nsc.Instance->OnCreate();
					}
					nsc.Instance->OnUpdate(ts);
				});
			}

			m_Registry.view<ParticleComponent>().each([=](auto entity, auto& pc)
			{
				pc.Particle.OnUpdate(ts);
			});

			// Animation
			auto& animView = m_Registry.view<AnimationComponent>();
			for (auto e : animView)
			{
				auto ac = animView.get<AnimationComponent>(e);
				if(!ac.Animations.empty())
					ac.Animations[ac.CurrentAnimation]->OnUpdateRuntime();
			}

			Renderer2D::Begin(editorCamera);
			const auto& audioView = m_Registry.view<TransformComponent, AudioComponent>();
			for (auto entity : audioView)
			{
				auto& [tc, ac] = audioView.get<TransformComponent, AudioComponent>(entity);
				if (std::shared_ptr<AudioSource> &audio = AssetManager::GetAsset<AudioSource>(ac.Audio))
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

			bool isMainCameraListening = false;
			const auto& audioListenerView = m_Registry.view<TransformComponent, AudioListenerComponent>();
			for (const auto entity : audioListenerView)
			{
				auto& [tc, al] = audioListenerView.get<TransformComponent, AudioListenerComponent>(entity);
				if(al.Enable)
					al.Listener.Set(tc.Translation, glm::vec3(0.0f), tc.GetForward(), tc.GetUp());
				isMainCameraListening = al.Enable;
			}

			if(!isMainCameraListening)
				editorCamera.UpdateAudioListener(ts);

			m_PhysicsScene->Simulate(ts);
			m_Physics2D->Simulate(ts);
		}

		RenderScene(editorCamera);
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
			std::shared_ptr<AudioSource>& audio = AssetManager::GetAsset<AudioSource>(ac.Audio);

			if (ac.PlayAtStart)
				audio->Play();
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
			
			if (const std::shared_ptr<AudioSource>& audio = AssetManager::GetAsset<AudioSource>(ac.Audio))
				audio->Stop();
		}
	}

	void Scene::RenderScene(const SceneCamera& camera, const TransformComponent& cameraTransform)
	{
		UpdateTransform();
		Renderer2D::Begin(camera, cameraTransform.GetTransform());

		auto& particles = m_Registry.view<TransformComponent, ParticleComponent>();
		for (auto entity : particles)
		{
			auto& [tc, pc] = particles.get<TransformComponent, ParticleComponent>(entity);

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
				auto& [sc, ac] = animView.get<SpriteRenderer2DComponent, AnimationComponent>(entity);
				if (!ac.Animations.empty())
				{
					ac.Animations[ac.CurrentAnimation]->OnUpdateRuntime();
					if (ac.Animations[ac.CurrentAnimation]->HasFrame())
						sc.Texture = ac.Animations[ac.CurrentAnimation]->GetCurrentValue();
				}
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

		Renderer2D::End();

		glEnable(GL_CULL_FACE);
		glCullFace(GL_BACK);

		const auto& lightView = m_Registry.view<TransformComponent, LightComponent>();

		const auto& meshView = m_Registry.view<TransformComponent, StaticMeshComponent>();
		for (auto& entity : meshView)
		{
			auto& [tc, mesh] = meshView.get<TransformComponent, StaticMeshComponent>(entity);

			if (AssetManager::GetAssetType(mesh.Handle) == AssetType::StaticMesh)
			{
				std::shared_ptr<Model> model = AssetManager::GetAsset<Model>(mesh.Handle);

				for (auto& light : lightView)
				{
					auto& [lightTransform, lc] = lightView.get<TransformComponent, LightComponent>(light);
					lc.Light->GetShadow()->OnAttachTexture(model->GetMaterial()->m_Shader);
					lc.Light->OnRender(lightTransform);
				}

				model->SetTransform(tc.GetTransform());
				model->Draw((int)entity);
			}
		}

		glDisable(GL_CULL_FACE);
	}

	void Scene::RenderScene(const EditorCamera& camera)
	{
		UpdateTransform();
		Renderer2D::Begin(camera);

		// Particle
		auto& particles = m_Registry.view<TransformComponent, ParticleComponent>();
		for (auto entity : particles)
		{
			auto& [tc, pc] = particles.get<TransformComponent, ParticleComponent>(entity);

			for (int i = 0; i < 5; i++)
			{
				pc.Particle.Emit(pc,
					glm::vec3(tc.Translation.x, tc.Translation.y, tc.Translation.z + pc.ZAxis),
					tc.Scale, pc.Rotation, static_cast<int>(entity)
				);
			}

			pc.Particle.OnRender();
		}

		auto& quads = m_Registry.view<TransformComponent, SpriteRenderer2DComponent>();
		std::vector<entt::entity> quadEntitties(quads.begin(), quads.end());

		std::sort(quadEntitties.begin(), quadEntitties.end(), [=](const entt::entity& a, const entt::entity& b)
			{
				const auto& objA = m_Registry.get<TransformComponent>(a);
				const auto& objB = m_Registry.get<TransformComponent>(b);
				return glm::length(camera.GetPosition().z - objA.Translation.z) > glm::length(camera.GetPosition().z - objB.Translation.z);
			});

		for (const entt::entity& entity : quadEntitties)
		{
			auto& [tc, sprite] = quads.get<TransformComponent, SpriteRenderer2DComponent>(entity);
			Renderer2D::DrawSprite(tc.GetTransform(), sprite, static_cast<int>(entity));
		}

		const auto& animView = m_Registry.view<SpriteRenderer2DComponent, AnimationComponent>();
		for (auto& entity : animView)
		{
			auto& [sc, ac] = animView.get<SpriteRenderer2DComponent, AnimationComponent>(entity);
			if (!ac.Animations.empty())
			{
				if(ac.Animations[ac.CurrentAnimation]->HasFrame())
					sc.Texture = ac.Animations[ac.CurrentAnimation]->GetCurrentValue();
			}
		}

		auto& circles = m_Registry.view<TransformComponent, CircleRendererComponent>();
		std::vector<entt::entity> circleEntities(circles.begin(), circles.end());

		std::sort(circleEntities.begin(), circleEntities.end(), [=](const entt::entity& a, const entt::entity& b)
			{
				const auto& objA = m_Registry.get<TransformComponent>(a);
				const auto& objB = m_Registry.get<TransformComponent>(b);
				return glm::length(camera.GetPosition().z - objA.Translation.z) > glm::length(camera.GetPosition().z - objB.Translation.z);
			});

		for (auto& entity : circleEntities)
		{
			auto& [transform, circle] = circles.get<TransformComponent, CircleRendererComponent>(entity);
			Renderer2D::DrawCircle(transform.GetTransform(), circle.Color, circle.Thickness, circle.Fade,static_cast<int>(entity));
		}

		// Text
		const auto& textView = m_Registry.view<TransformComponent, TextComponent>();
		for (auto entity : textView)
		{
			auto [tc, text] = textView.get<TransformComponent, TextComponent>(entity);
			Renderer2D::DrawString(text.TextString, tc.GetTransform(), text, static_cast<int>(entity));
		}

		Renderer2D::End();

		glEnable(GL_CULL_FACE);
		glCullFace(GL_BACK);

		const auto& lightView = m_Registry.view<TransformComponent, LightComponent>();
		
		const auto& meshView = m_Registry.view<TransformComponent, StaticMeshComponent>();
		for (auto& entity : meshView)
		{
			auto& [tc, mesh] = meshView.get<TransformComponent, StaticMeshComponent>(entity);

			if (AssetManager::GetAssetType(mesh.Handle) == AssetType::StaticMesh)
			{
				std::shared_ptr<Model> model = AssetManager::GetAsset<Model>(mesh.Handle);

				for (auto& light : lightView)
				{
					auto& [lightTransform, lc] = lightView.get<TransformComponent, LightComponent>(light);
					lc.Light->GetShadow()->OnAttachTexture(model->GetMaterial()->m_Shader);
					lc.Light->OnRender(lightTransform);
				}

				model->SetTransform(tc.GetTransform());
				model->Draw((int)entity);
			}
		}

		glDisable(GL_CULL_FACE);
	}
	
	void Scene::OnShadowRender()
	{
		glEnable(GL_CULL_FACE);
		glCullFace(GL_FRONT);
		const auto& dirLight = m_Registry.view<TransformComponent, LightComponent>();
		for (auto& light : dirLight)
		{
			auto& [lightTransform, lc] = dirLight.get<TransformComponent, LightComponent>(light);
			lc.Light->GetShadow()->BindFramebuffer();

			const auto& meshView = m_Registry.view<TransformComponent, StaticMeshComponent>();
			for (auto& entity : meshView)
			{
				auto& [tc, mesh] = meshView.get<TransformComponent, StaticMeshComponent>(entity);

				if (AssetManager::GetAssetType(mesh.Handle) == AssetType::StaticMesh)
				{
					std::shared_ptr<Model> model = AssetManager::GetAsset<Model>(mesh.Handle);
					lc.Light->GetShadow()->OnRenderBegin(lightTransform, tc.GetTransform());
					model->SetTransform(tc.GetTransform());
					model->Draw();
					lc.Light->GetShadow()->OnRenderEnd();
				}
			}

			lc.Light->GetShadow()->UnbindFramebuffer();
		}
		glCullFace(GL_BACK);
		glDisable(GL_CULL_FACE);
	}

	void Scene::UpdateTransform()
	{
		auto& t = m_Registry.view<TransformComponent, TreeNodeComponent>();
		for (auto e : t)
		{
			auto& [tc, tree] = t.get<TransformComponent, TreeNodeComponent>(e);
			if (tree.Parent)
			{
				Entity parent = GetEntityWithUUID(tree.Parent);
				const auto& parentTc = parent.GetComponent<TransformComponent>();

				glm::vec3 retScale = glm::vec3(1.0f);

				glm::vec3 pTranslation, pRotation, pScale;
				Math::DecomposeTransformEuler(parentTc.GetTransform(), pTranslation, pRotation, pScale);
			}
		}
	}

	void Scene::OnViewportResize(const uint32_t width, const uint32_t height)
	{
		const auto& view = m_Registry.view<CameraComponent>();
		for (auto& e : view)
		{
			auto& cc = view.get<CameraComponent>(e);
			if(cc.Primary)
				cc.Camera.SetViewportSize(width, height);
		}

		m_ViewportWidth = width;
		m_ViewportHeight = height;
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
	OGN_REG_COMPONENT(TreeNodeComponent)
	OGN_REG_COMPONENT(TagComponent)
	OGN_REG_COMPONENT(TransformComponent)
	OGN_REG_COMPONENT(AudioComponent)
	OGN_REG_COMPONENT(AudioListenerComponent)
	OGN_REG_COMPONENT(AnimationComponent)
	OGN_REG_COMPONENT(SpriteRenderer2DComponent)
	OGN_REG_COMPONENT(SpriteSheet2DComponent)
	OGN_REG_COMPONENT(LightComponent)
	OGN_REG_COMPONENT(StaticMeshComponent)
	OGN_REG_COMPONENT(TextComponent)
	OGN_REG_COMPONENT(CircleRendererComponent)
	OGN_REG_COMPONENT(NativeScriptComponent)
	OGN_REG_COMPONENT(ScriptComponent)
	OGN_REG_COMPONENT(Rigidbody2DComponent)
	OGN_REG_COMPONENT(BoxCollider2DComponent)
	OGN_REG_COMPONENT(CircleCollider2DComponent)
	OGN_REG_COMPONENT(RevoluteJoint2DComponent)
	OGN_REG_COMPONENT(ParticleComponent)
	OGN_REG_COMPONENT(RigidbodyComponent)
	OGN_REG_COMPONENT(BoxColliderComponent)
	OGN_REG_COMPONENT(SphereColliderComponent)
	OGN_REG_COMPONENT(CapsuleColliderComponent)

	template <>
	void Scene::OnComponentAdded<CameraComponent>(Entity entity, CameraComponent& component)
	{
		if (m_ViewportWidth > 0 && m_ViewportHeight > 0)
			component.Camera.SetViewportSize(m_ViewportWidth, m_ViewportHeight);
	}
}
