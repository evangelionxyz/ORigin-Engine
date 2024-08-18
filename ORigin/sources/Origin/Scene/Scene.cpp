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
#include "Lighting.h"
#include "Entity.h"
#include "ScriptableEntity.h"
#include "Origin/Physics/PhysicsEngine.h"
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/quaternion.hpp>
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

    void Scene::PreRender(const Camera &camera, Timestep ts)
    {
        OGN_PROFILER_FUNCTION();

        const auto &view = m_Registry.view<IDComponent, TransformComponent>();
        for (auto [entity, idc, tc]: view.each())
        {
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

        for (auto [e, tc, mesh] : m_Registry.view<TransformComponent, MeshComponent>().each())
        {
            if (mesh.Data)
            {
                mesh.Animator.UpdateAnimation(ts, mesh.PlaybackSpeed);
            }
        }

		OnRenderShadow();
		m_UIRenderer->RenderFramebuffer();
    }

    void Scene::PostRender(const Camera &camera, Timestep ts)
    {

    }

    void Scene::Update(Timestep ts)
	{
        const auto &view = m_Registry.view<TransformComponent>();
        for (auto e : view)
        {
            Entity entity = { e, this };
            auto &tc = entity.GetComponent<TransformComponent>();
            if (entity.HasComponent<SpriteAnimationComponent>())
            {
                auto &ac = entity.GetComponent<SpriteAnimationComponent>();
                if (ac.State->IsCurrentAnimationExists())
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
            Entity entity { entityID, this };
            ScriptEngine::OnUpdateEntity(entity, time);
        });

        m_Registry.view<NativeScriptComponent>().each([this, time = ts](auto entityID, auto &nsc)
        {
            nsc.Instance->OnUpdate(time);
        });
    }

    void Scene::UpdatePhysics(Timestep ts)
    {
        PhysicsEngine::Simulate(ts, this);
        m_Physics2D->Simulate(ts);
    }

	void Scene::OnUpdateRuntime(Timestep ts)
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
			const std::shared_ptr<AudioSource> &audio = AssetManager::GetAsset<AudioSource>(ac.Audio);
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

        PhysicsEngine::OnSimulateStart(this);
        m_Physics2D->OnSimulationStart();
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

	void Scene::OnUpdateEditor(const Camera &camera, Timestep ts, entt::entity selectedID)
	{
		Update(ts);
		RenderScene(camera);
        RenderStencilScene(camera, selectedID);
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
        RenderStencilScene(camera, selectedID);
	}

    void Scene::RenderScene(const Camera &camera)
    {
        OGN_PROFILER_RENDERING();
        glEnable(GL_DEPTH_TEST);

        Renderer2D::Begin(camera);
        const auto &view = m_Registry.view<TransformComponent>();
        for (auto e : view)
        {
            Entity entity { e, this };
            auto &tc = view.get<TransformComponent>(e);

            if (!tc.Visible)
            {
                continue;
            }

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
                Renderer2D::DrawSprite(tc.GetTransform(), src);
            }

            if (entity.HasComponent<CircleRendererComponent>())
            {
                auto &cc = entity.GetComponent<CircleRendererComponent>();
                Renderer2D::DrawCircle(tc.GetTransform(), cc.Color, cc.Thickness, cc.Fade);
            }

            // Particles
            if (entity.HasComponent<ParticleComponent>())
            {
                auto &pc = entity.GetComponent<ParticleComponent>();
                for (int i = 0; i < 5; i++)
                {
                    pc.Particle.Emit(pc,
                        { tc.WorldTranslation.x, tc.WorldTranslation.y, tc.WorldTranslation.z + pc.ZAxis },
                        tc.WorldScale, pc.Rotation);
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
                    glm::vec3 centerOffset = glm::vec3(text.Size.x / 2.0f, -text.Size.y / 2.0f + 1.0f, 0.0f);
                    glm::vec3 scaledOffset = tc.WorldScale * centerOffset;
                    glm::vec3 rotatedOffset = glm::toMat3(tc.WorldRotation) * scaledOffset;

                    text.Position = tc.WorldTranslation - rotatedOffset;

                    transform = glm::translate(glm::mat4(1.0f), text.Position)
                        * glm::toMat4(tc.WorldRotation)
                        * glm::scale(glm::mat4(1.0f), tc.WorldScale);
                }

                if (text.FontHandle != 0)
                {
                    Renderer2D::DrawString(text.TextString, transform, text);
                }
            }
        }

        Renderer2D::End();

        const auto meshView = m_Registry.view<TransformComponent, MeshComponent>();
        for (auto [e, tc, mesh] : meshView.each())
        {
            // Render
            if (mesh.Data && tc.Visible)
            {
                Shader *shader = Renderer::GetShader("AnimatedMesh").get();
                shader->Enable();

                glActiveTexture(0);
                mesh.Data->DiffuseTexture->Bind(0);
                shader->SetInt("uTexture", mesh.Data->DiffuseTexture->GetTextureID());

                shader->SetMatrix("viewProjection", camera.GetViewProjection());
                shader->SetMatrix("model", tc.GetTransform());

                shader->SetMatrix("boneTransforms", 
                    mesh.Animator.m_FinalBoneMatrices[0], 
                    mesh.Animator.m_FinalBoneMatrices.size());

                mesh.Data->vertexArray->Bind();
                glDrawElements(GL_TRIANGLES, mesh.Data->indices.size(), GL_UNSIGNED_INT, nullptr);

                shader->Disable();
            }
        }

        const auto &lightView = m_Registry.view<TransformComponent, LightComponent>();
        const auto &sMeshView = m_Registry.view<TransformComponent, StaticMeshComponent>();
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

            MeshRenderer::Begin(camera);
            for (auto e : sMeshView)
            {
                const auto [tc, mc] = sMeshView.get<TransformComponent, StaticMeshComponent>(e);
                if (!tc.Visible)
                    continue;

                std::shared_ptr<Material> material;
                if (mc.HMaterial) material = AssetManager::GetAsset<Material>(mc.HMaterial);
                else material = Renderer::GetMaterial("Mesh");
                switch (mc.mType)
                {
                case StaticMeshComponent::Type::Cube:
                    MeshRenderer::DrawCube(tc.GetTransform(), material.get());
                    break;
                case StaticMeshComponent::Type::Sphere:
                    MeshRenderer::DrawSphere(tc.GetTransform(), material.get());
                    break;
                case StaticMeshComponent::Type::Capsule:
                    MeshRenderer::DrawCapsule(tc.GetTransform(), material.get());
                    break;
                case StaticMeshComponent::Type::Default:
                    if (mc.Data)
                    {
                        MeshRenderer::DrawMesh(camera.GetViewProjection(), tc.GetTransform(), mc.Data->vertexArray);
                    }
                    break;
                }
            }
            MeshRenderer::End();
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

            // First pass: Render all objects and mark the selected object in the stencil buffer

            glStencilFunc(GL_ALWAYS, 1, 0xFF);
            glStencilMask(0xFF);

            // 2D Objects
            Renderer2D::Begin(camera);
            if (entity.HasComponent<SpriteRenderer2DComponent>())
            {
                SpriteRenderer2DComponent &sc = entity.GetComponent<SpriteRenderer2DComponent>();
                if (entity.HasComponent<SpriteAnimationComponent>())
                {
                    SpriteAnimationComponent &ac = entity.GetComponent<SpriteAnimationComponent>();
                    if (ac.State->IsCurrentAnimationExists())
                    {
                        if (ac.State->GetAnimation()->HasFrame())
                        {
                            std::shared_ptr<SpriteAnimation> &anim = ac.State->GetAnimation();
                            sc.Texture = anim->GetCurrentFrame().Handle;
                            sc.Min = anim->GetCurrentFrame().Min;
                            sc.Max = anim->GetCurrentFrame().Max;
                        }
                    }
                }

                Renderer2D::DrawSprite(tc.GetTransform(), sc);
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
            //if(entity.HasComponent<MeshComponent>())
            //{
            //    MeshComponent &mesh = entity.GetComponent<MeshComponent>();
            //    if (mesh.Data && tc.Visible)
            //    {
            //        Shader *shader = Renderer::GetShader("AnimatedMesh").get();
            //        shader->Enable();
            //        glActiveTexture(0);
            //        Renderer::WhiteTexture->Bind(0);
            //        shader->SetInt("uTexture", Renderer::WhiteTexture->GetTextureID());
            //        shader->SetMatrix("viewProjection", camera.GetViewProjection());
            //        shader->SetMatrix("model", tc.GetTransform());

            //        //auto transforms = mesh.Animator.GetFinalBoneMatrices();
            //        //shader->SetMatrix("boneTransforms", transforms[0], transforms.size());

            //        mesh.Data->vertexArray->Bind();
            //        glDrawElements(GL_TRIANGLES, mesh.Data->indices.size(), GL_UNSIGNED_INT, nullptr);

            //        shader->Disable();
            //    }
            //}

            if (entity.HasComponent<StaticMeshComponent>())
            {
                StaticMeshComponent &mc = entity.GetComponent<StaticMeshComponent>();
                std::shared_ptr<Material> material;
                if (mc.HMaterial) material = AssetManager::GetAsset<Material>(mc.HMaterial);
                else material = Renderer::GetMaterial("Mesh");

                MeshRenderer::Begin(camera);
                switch (mc.mType)
                {
                case StaticMeshComponent::Type::Cube:
                    MeshRenderer::DrawCube(tc.GetTransform(), material.get());
                    break;
                case StaticMeshComponent::Type::Sphere:
                    MeshRenderer::DrawSphere(tc.GetTransform(), material.get());
                    break;
                case StaticMeshComponent::Type::Capsule:
                    MeshRenderer::DrawCapsule(tc.GetTransform(), material.get());
                    break;
                case StaticMeshComponent::Type::Default:
                    if (mc.Data)
                    {
                        MeshRenderer::DrawMesh(camera.GetViewProjection(), tc.GetTransform(), mc.Data->vertexArray);
                    }
                    break;
                }
                MeshRenderer::End();
            }

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
            Renderer2D::Begin(camera, outlineShader);
            if (entity.HasComponent<SpriteRenderer2DComponent>())
            {
                SpriteRenderer2DComponent &sc = entity.GetComponent<SpriteRenderer2DComponent>();
                if (entity.HasComponent<SpriteAnimationComponent>())
                {
                    SpriteAnimationComponent &ac = entity.GetComponent<SpriteAnimationComponent>();
                    if (ac.State->IsCurrentAnimationExists())
                    {
                        if (ac.State->GetAnimation()->HasFrame())
                        {
                            std::shared_ptr<SpriteAnimation> &anim = ac.State->GetAnimation();
                            sc.Texture = anim->GetCurrentFrame().Handle;
                            sc.Min = anim->GetCurrentFrame().Min;
                            sc.Max = anim->GetCurrentFrame().Max;
                        }
                    }
                }
                Renderer2D::DrawSprite(scaledTransform, sc);
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

            /*if (entity.HasComponent<MeshComponent>())
            {
                MeshComponent &mesh = entity.GetComponent<MeshComponent>();
                if (mesh.Data && tc.Visible)
                {
                    outlineShader->SetMatrix("viewProjection", camera.GetViewProjection() * scaledTransform);
                    mesh.Data->vertexArray->Bind();
                    glDrawElements(GL_TRIANGLES, mesh.Data->indices.size(), GL_UNSIGNED_INT, nullptr);
                }
            }*/

            MeshRenderer::Begin(camera, outlineShader);
            if (entity.HasComponent<StaticMeshComponent>())
            {
                StaticMeshComponent &mc = entity.GetComponent<StaticMeshComponent>();
                switch (mc.mType)
                {
                case StaticMeshComponent::Type::Cube:
                    MeshRenderer::DrawCube(scaledTransform, glm::vec4(1.0f));
                    break;
                case StaticMeshComponent::Type::Sphere:
                    MeshRenderer::DrawSphere(scaledTransform, glm::vec4(1.0f));
                    break;
                case StaticMeshComponent::Type::Capsule:
                    MeshRenderer::DrawCapsule(scaledTransform, glm::vec4(1.0f));
                    break;
                case StaticMeshComponent::Type::Default:
                    if (mc.Data)
                    {
                        MeshRenderer::DrawMesh(camera.GetViewProjection(), scaledTransform, mc.Data->vertexArray, outlineShader);
                    }
                    break;
                }
                MeshRenderer::End();
            }

            outlineShader->Disable();

            glStencilMask(0xFF);
            glStencilFunc(GL_ALWAYS, 0, 0xFF);
            glEnable(GL_DEPTH_TEST);
#pragma endregion
        }

        glDisable(GL_STENCIL_TEST);
    }
	
	void Scene::OnRenderShadow()
	{
        glEnable(GL_DEPTH_TEST);
		const auto &dirLight = m_Registry.view<TransformComponent, LightComponent>();
		for (auto &light : dirLight)
		{
			const auto &[lightTC, lc] = dirLight.get<TransformComponent, LightComponent>(light);
			if (!lightTC.Visible)
			{
                continue;
			}
		
	        float size = lc.Light->OrthoSize;
			glm::mat4 lightProjection = glm::ortho(-size, size, -size, size, lc.Light->NearPlane, lc.Light->FarPlane);
			glm::mat4 lightView = glm::lookAt(lightTC.WorldTranslation,
											  glm::eulerAngles(lightTC.Rotation),
                                              glm::vec3(0.0f, 1.0f, 0.0f));

            glm::mat4 lightViewProjection = lightProjection * lightView;
			lc.Light->GetShadow().ViewProjection = lightProjection * lightView;

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

        return { entt::null, nullptr };
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

    OGN_ADD_COMPONENT(IDComponent);
    OGN_ADD_COMPONENT(TagComponent);
    OGN_ADD_COMPONENT(TransformComponent);
    OGN_ADD_COMPONENT(UIComponent);
    OGN_ADD_COMPONENT(AudioListenerComponent);

    template<>
    void Scene::OnComponentAdded<AudioComponent>(Entity entity, AudioComponent &component)
    {
    }

    template<>
    void Scene::OnComponentAdded<SpriteRenderer2DComponent>(Entity entity, SpriteRenderer2DComponent &component)
    {
    }

    template<>
    void Scene::OnComponentAdded<SpriteAnimationComponent>(Entity entity, SpriteAnimationComponent &component)
    {
    }

	OGN_ADD_COMPONENT(MeshComponent);
	OGN_ADD_COMPONENT(MeshRendererComponent);
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
        if(!entity.HasComponent<RigidbodyComponent>()) entity.AddComponent<RigidbodyComponent>();
    }

	template<>
	void Scene::OnComponentAdded(Entity entity, StaticMeshComponent &component)
	{
	}

    template<>
    void Scene::OnComponentAdded(Entity entity, SphereColliderComponent &component)
    {
        if(!entity.HasComponent<RigidbodyComponent>()) entity.AddComponent<RigidbodyComponent>();
    }

    template<>
    void Scene::OnComponentAdded(Entity entity, CapsuleColliderComponent &component)
    {
        if(!entity.HasComponent<RigidbodyComponent>()) entity.AddComponent<RigidbodyComponent>();
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
