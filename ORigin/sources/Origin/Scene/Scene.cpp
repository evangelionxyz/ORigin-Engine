// Copyright (c) Evangelion Manuhutu | ORigin Engine

#include "pch.h"
#include "Origin/Audio/AudioEngine.h"
#include "Origin/Audio/AudioSource.h"
#include "Origin/Profiler/Profiler.h"
#include "Origin/Animation/Animation.h"
#include "Origin/Physics/Contact2DListener.h"
#include "Origin/Physics/Physics2D.h"
#include "Origin/Renderer/Renderer.h"
#include "Origin/Renderer/Renderer2D.h"
#include "Origin/Scripting/ScriptEngine.h"
#include "Origin/Asset/AssetManager.h"
#include "Origin/Core/Log.h"
#include "Scene.h"
#include "Entity.h"
#include "Lighting.h"
#include "EntityManager.h"
#include "ScriptableEntity.h"

#include <glm/glm.hpp>

namespace origin
{
	class LightComponent;

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
		auto idView = srcSceneRegistry.view<IDComponent>();
		for (auto e : idView)
		{
			auto idc = srcSceneRegistry.get<IDComponent>(e);
			const auto name = srcSceneRegistry.get<TagComponent>(e).Tag;
			newEntity = EntityManager::CreateEntityWithUUID(idc.ID, name, newScene.get());
			auto &eIDC = newEntity.GetComponent<IDComponent>();
			eIDC.Parent = idc.Parent;

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
				return { e.second, this };
		}

		return Entity();
	}

	Entity Scene::FindEntityByName(std::string_view name)
	{
		OGN_PROFILER_SCENE();

		auto view = m_Registry.view<TagComponent>();
		for (auto entity : view)
		{
			const TagComponent& tc = view.get<TagComponent>(entity);
			if (tc.Tag == name)
				return { entity, this };
		}

		return Entity();
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
				if (entity.IsValid())
				{
					ScriptEngine::OnUpdateEntity(entity, ts);
				}
			}

			m_Registry.view<NativeScriptComponent>().each([time = ts](auto entity, auto &nsc)
			{
				if (nsc.Instance)
					nsc.Instance->OnUpdate(time);
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
					al.Listener.Set(tc.Translation, glm::vec3(0.0f), tc.GetForward(), tc.GetUp());
			}

			m_Physics2D->Simulate(ts);
		}

	}

	void Scene::DestroyEntity(Entity entity)
	{
		UUID uuid = entity.GetUUID();

		m_Registry.view<IDComponent>().each([&](entt::entity e, auto idc)
		{
				if (EntityManager::IsParent(idc.ID, uuid, this))
				{
					Entity entt = { e, this };
					m_Registry.destroy(e);
					m_EntityStorage.erase(std::remove_if(m_EntityStorage.begin(), m_EntityStorage.end(),
					[&](auto e)
					{
						return e.first == idc.ID;
					}), m_EntityStorage.end());
				}
		});
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
					return { entity, this };
			}
		}
		return {};
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
				UpdateTransform();
				RenderScene(cc.Camera, tc);

				break;
			}
		}
	}

	void Scene::OnRuntimeStart()
	{
		OGN_PROFILER_SCENE();

		m_Running = true;

		UpdateTransform();

		ScriptEngine::SetSceneContext(this);
		auto scriptView = m_Registry.view<ScriptComponent>();
		for (auto e : scriptView)
		{
			Entity entity = { e, this };
			ScriptEngine::OnCreateEntity(entity);
		}

		m_Registry.view<NativeScriptComponent>().each([this](auto entity, auto &nsc)
		{
			if(!nsc.Instance)
			{
				nsc.Instance = nsc.InstantiateScript();
				nsc.Instance->m_Entity = Entity { entity, this };	
			}
		});

		auto audioView = m_Registry.view<AudioComponent>();
		for (auto& e : audioView)
		{
			auto& ac = audioView.get<AudioComponent>(e);
			const std::shared_ptr<AudioSource> &audio = AssetManager::GetAsset<AudioSource>(ac.Audio);
			if (ac.PlayAtStart)
				audio->Play();
		}

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
				const glm::vec2 &ortho = cc.Camera.GetOrthographicSize();
				m_UIRenderer->CreateFramebuffer(vp.x, vp.y, ortho.x, ortho.y);
			}
		});
	}

	void Scene::OnRuntimeStop()
	{
		OGN_PROFILER_SCENE();

		m_Running = false;
		ScriptEngine::ClearSceneContext();
		m_Registry.view<NativeScriptComponent>().each([this](auto entity, auto& nsc)
		{
			nsc.DestroyScript(&nsc);
		});

		const auto& audioView = m_Registry.view<AudioComponent>();

		for (auto& e : audioView)
		{
			auto& ac = audioView.get<AudioComponent>(e);
			if (const std::shared_ptr<AudioSource>& audio = AssetManager::GetAsset<AudioSource>(ac.Audio))
				audio->Stop();
		}

		m_Physics2D->OnSimulationStop();

		m_UIRenderer->Unload();
	}

	void Scene::OnEditorUpdate(Timestep ts, EditorCamera& editorCamera)
	{
		OGN_PROFILER_RENDERING();

		m_Registry.view<NativeScriptComponent>().each([time = ts](auto entity, auto& nsc)
		{
			nsc.Instance->OnUpdate(time);
		});

		m_Registry.view<ParticleComponent>().each([time = ts](auto entity, auto& pc)
		{
			pc.Particle.OnUpdate(time);
		});

		// Animation
		const auto& animView = m_Registry.view<SpriteAnimationComponent>();
		for (const auto entity : animView)
		{
			auto& ac = animView.get<SpriteAnimationComponent>(entity);
			if(ac.State->IsCurrentAnimationExists())
				ac.State->OnUpdateEditor(ts);
		}

		// Audio Update
		Renderer2D::Begin(editorCamera);
		const auto& audioView = m_Registry.view<TransformComponent, AudioComponent>();
		for (auto entity : audioView)
		{
			const auto& [tc, ac] = audioView.get<TransformComponent, AudioComponent>(entity);
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
		editorCamera.UpdateAudioListener(ts);

		UpdateTransform();
		RenderScene(editorCamera);
	}

	void Scene::OnUpdateSimulation(Timestep ts, EditorCamera& editorCamera)
	{
		OGN_PROFILER_RENDERING();

		if (!m_Paused || m_StepFrames-- > 0)
		{
			// Update Scripts
			m_Registry.view<ScriptComponent>().each([this, time = ts](auto entityID, auto& sc)
			{
				Entity entity{ entityID, this };
				ScriptEngine::OnUpdateEntity(entity, time);
			});

			m_Registry.view<NativeScriptComponent>().each([this, time = ts](auto entityID, auto& nsc)
			{
				nsc.Instance->OnUpdate(time);
			});

			m_Registry.view<ParticleComponent>().each([this, time = ts](auto entity, auto& pc)
			{
				pc.Particle.OnUpdate(time);
			});

			// Animation
			const auto& animView = m_Registry.view<SpriteAnimationComponent>();
			for (auto e : animView)
			{
				auto ac = animView.get<SpriteAnimationComponent>(e);
				if (ac.State->IsCurrentAnimationExists())
					ac.State->OnUpdateRuntime(ts);
			}

			Renderer2D::Begin(editorCamera);
			const auto& audioView = m_Registry.view<TransformComponent, AudioComponent>();
			for (auto entity : audioView)
			{
				const auto& [tc, ac] = audioView.get<TransformComponent, AudioComponent>(entity);
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

			bool isMainCameraListening = false;
			const auto& audioListenerView = m_Registry.view<TransformComponent, AudioListenerComponent>();
			for (const auto entity : audioListenerView)
			{
				auto [tc, al] = audioListenerView.get<TransformComponent, AudioListenerComponent>(entity);
				if(al.Enable)
					al.Listener.Set(tc.Translation, glm::vec3(0.0f), tc.GetForward(), tc.GetUp());
				isMainCameraListening = al.Enable;
			}

			if(!isMainCameraListening)
				editorCamera.UpdateAudioListener(ts);

			m_Physics2D->Simulate(ts);
		}

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

		m_Registry.view<NativeScriptComponent>().each([this](auto entity, auto& nsc)
		{
				nsc.Instance = nsc.InstantiateScript();
				nsc.Instance->m_Entity = Entity { entity, this };
		});

		m_Physics2D->OnSimulationStart();

		// Audio
		const auto& audioView = m_Registry.view<AudioComponent>();
		for (auto& e : audioView)
		{
			auto& ac = audioView.get<AudioComponent>(e);
			const std::shared_ptr<AudioSource> &audio = AssetManager::GetAsset<AudioSource>(ac.Audio);
			if (ac.PlayAtStart)
				audio->Play();
		}
	}

	void Scene::OnSimulationStop()
	{
		OGN_PROFILER_SCENE();

		m_Running = false;

		ScriptEngine::ClearSceneContext();
		m_Registry.view<NativeScriptComponent>().each([this](auto entity, auto& nsc)
		{
			nsc.DestroyScript(&nsc);
		});

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

	void Scene::RenderScene(const EditorCamera &camera)
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

		Renderer2D::Begin(camera);
		glDisable(GL_DEPTH_TEST);

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
						if (primaryCam.IsValid())
						{
							const auto &cc = primaryCam.GetComponent<CameraComponent>().Camera;
							const auto &ccTC = primaryCam.GetComponent<TransformComponent>();
							const float ratio = cc.GetAspectRatio();
							glm::vec2 scale = glm::vec2(tc.WorldScale.x, tc.WorldScale.y * ratio);
							transform = glm::translate(glm::mat4(1.0f), { tc.WorldTranslation.x, tc.WorldTranslation.y, 0.0f })
								* glm::toMat4(glm::quat(ccTC.Rotation))
								* glm::scale(glm::mat4(1.0f), { tc.WorldScale.x, tc.WorldScale.y * ratio, 0.0 });

							invertedCamTransform = glm::inverse(cc.GetViewProjection());
						}
					}
					else
					{
						const float ratio = camera.GetAspectRatio();
						transform = glm::scale(tc.GetTransform(), { tc.WorldScale.x, tc.WorldScale.y * ratio, 0.0});
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



		// ===============================
		// 3D Scene
		glEnable(GL_DEPTH_TEST);

		const auto &lightView = m_Registry.view<TransformComponent, LightComponent>();
		const auto &modelView = m_Registry.view<TransformComponent, ModelComponent>();
		for (const auto entity : modelView)
		{
			const auto &[tc, mc] = modelView.get<TransformComponent, ModelComponent>(entity);
			if(!tc.Visible)
				continue;

			if (AssetManager::GetAssetType(mc.Handle) == AssetType::Model)
			{
				std::shared_ptr<Model> model = AssetManager::GetAsset<Model>(mc.Handle);

#if 0
				for (auto &light : lightView)
				{
					const auto &[lightTC, lc] = lightView.get<TransformComponent, LightComponent>(light);
					lc.Light->GetShadow()->OnAttachTexture(model->GetMaterial()->m_Shader);
					lc.Light->OnRender(lightTC);
				}
#endif

				model->SetTransform(tc.GetTransform());

				for (auto &mesh : model->GetMeshes())
				{
					Entity e = GetEntityWithUUID(mesh);
					StaticMeshComponent &sm = e.GetComponent<StaticMeshComponent>();
					sm.OMesh->Draw(tc.GetTransform(), (int)e);
				}
			}
		}
	}

	void Scene::RenderScene(const SceneCamera& camera, const TransformComponent& cameraTransform)
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

		Renderer2D::Begin(camera, cameraTransform.GetTransform());
		glDisable(GL_DEPTH_TEST);
		{
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
						const float ratio = camera.GetAspectRatio();
						transform = glm::scale(tc.GetTransform(), { tc.WorldScale.x, tc.WorldScale.y * ratio, 0.0f });
						invertedCamTransform = glm::inverse(camera.GetViewProjection());
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
		}

		Renderer2D::End();
	}
	
	void Scene::OnShadowRender()
	{
#if 0
		OGN_PROFILER_RENDERING();

		const auto& dirLight = m_Registry.view<TransformComponent, LightComponent>();
		for (auto& light : dirLight)
		{
			const auto& [lightTransform, lc] = dirLight.get<TransformComponent, LightComponent>(light);
			lc.Light->GetShadow()->BindFramebuffer();

			const auto& meshView = m_Registry.view<TransformComponent, StaticMeshComponent>();
			for (auto& entity : meshView)
			{
				const auto& [tc, mesh] = meshView.get<TransformComponent, StaticMeshComponent>(entity);

				if (AssetManager::GetAssetType(mesh.Handle) == AssetType::Model)
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
#endif
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
				const auto &ptc = GetEntityWithUUID(idc.Parent).GetComponent<TransformComponent>();
				glm::vec3 rotatedLocalPos = glm::rotate(glm::quat(ptc.WorldRotation), tc.Translation);
				tc.WorldTranslation = rotatedLocalPos + ptc.WorldTranslation;
				tc.WorldRotation = ptc.WorldRotation + tc.Rotation;
				tc.WorldScale = tc.Scale * ptc.WorldScale;
			}
			else
			{
				tc.WorldTranslation = tc.Translation;
				tc.WorldRotation = tc.Rotation;
				tc.WorldScale = tc.Scale;
			}
		}
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
				const glm::vec2 &ortho = cc.Camera.GetOrthographicSize();
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
	{
	}

#define OGN_REG_COMPONENT(components)\
template<>\
void Scene::OnComponentAdded<components>(Entity entity, components& component){}

	OGN_REG_COMPONENT(IDComponent)
	OGN_REG_COMPONENT(TagComponent)
	OGN_REG_COMPONENT(TransformComponent)
	OGN_REG_COMPONENT(UIComponent)
	OGN_REG_COMPONENT(AudioComponent)
	OGN_REG_COMPONENT(AudioListenerComponent)
	OGN_REG_COMPONENT(SpriteAnimationComponent)
	OGN_REG_COMPONENT(SpriteRenderer2DComponent)
	OGN_REG_COMPONENT(LightComponent)
	OGN_REG_COMPONENT(StaticMeshComponent)
	OGN_REG_COMPONENT(ModelComponent)
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

	template <>
	void Scene::OnComponentAdded<CameraComponent>(Entity entity, CameraComponent& component)
	{
		if (m_ViewportWidth > 0 && m_ViewportHeight > 0)
			component.Camera.SetViewportSize(m_ViewportWidth, m_ViewportHeight);
	}
}
