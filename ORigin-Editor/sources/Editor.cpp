// Copyright (c) 2022 Evangelion Manuhutu | ORigin Engine

#include "Editor.h"

#include "Origin/EntryPoint.h"
#include "panels/EditorTheme.h"
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "Origin/Utils/PlatformUtils.h"
#include "Origin/Scene/SceneSerializer.h"

#include <filesystem>

namespace Origin
{
	extern const std::filesystem::path g_AssetPath;

  Editor::Editor() : Layer("Editor") { }

  void Editor::OnAttach()
  {
    m_PlayButton = Texture2D::Create("resources/textures/playbutton.png");
    m_SimulateButton = Texture2D::Create("resources/textures/simulatebutton.png");
    m_StopButton = Texture2D::Create("resources/textures/stopbutton.png");

    FramebufferSpecification mainFramebufferSpec;
    mainFramebufferSpec.Attachments =
    {
      FramebufferTextureFormat::RGBA8,
      FramebufferTextureFormat::RED_INTEGER,
      FramebufferTextureFormat::Depth
    };

    mainFramebufferSpec.Width = 1280;
    mainFramebufferSpec.Height = 720;
    m_Framebuffer = Framebuffer::Create(mainFramebufferSpec);

		FramebufferSpecification gameFramebufferSpec;
    gameFramebufferSpec.Attachments =
    {
      FramebufferTextureFormat::RGBA8,
      FramebufferTextureFormat::Depth
    };

    gameFramebufferSpec.Width = 1280;
    gameFramebufferSpec.Height = 720;
    m_GameFramebuffer = Framebuffer::Create(gameFramebufferSpec);

		m_EditorCamera = EditorCamera(30.0f, 1.778f, 0.1f, 1000.0f);

		m_EditorCamera.SetDistance(15.0f);
		m_EditorCamera.SetPosition(glm::vec3(-11.0f, 5.5f, 12.0f));
		m_EditorCamera.SetYaw(0.7f);
		m_EditorCamera.SetPitch(0.350f);

    m_ActiveScene = std::make_shared<Scene>();

    auto commandLineArgs = Application::Get().GetSpecification().CommandLineArgs;
    if (commandLineArgs.Count > 1)
    {
      auto sceneFilepath = commandLineArgs[1];
      OpenScene(sceneFilepath);
    }
  }

  void Editor::OnEvent(Event& e)
  {
    m_EditorCamera.OnEvent(e);
    EventDispatcher dispatcher(e);
    dispatcher.Dispatch<MouseMovedEvent>(OGN_BIND_EVENT_FN(Editor::OnMouseMovedEvent));
    dispatcher.Dispatch<WindowResizeEvent>(OGN_BIND_EVENT_FN(Editor::OnWindowResize));
    dispatcher.Dispatch<KeyPressedEvent>(OGN_BIND_EVENT_FN(Editor::OnKeyPressed));
    dispatcher.Dispatch<MouseButtonPressedEvent>(OGN_BIND_EVENT_FN(Editor::OnMouseButtonPressed));
  }

  void Editor::OnUpdate(Timestep time)
  {
    m_Time += time.GetSeconds();
    m_SelectedEntity = m_SceneHierarchy.GetSelectedEntity();

    // Resize
    if (const FramebufferSpecification spec = m_Framebuffer->GetSpecification();
      m_SceneViewportSize.x > 0.0f && m_SceneViewportSize.y > 0.0f && (m_SceneViewportSize.x != spec.Width || m_SceneViewportSize.y != spec.Height))
    {
      m_Framebuffer->Resize(static_cast<uint32_t>(m_SceneViewportSize.x), static_cast<uint32_t>(m_SceneViewportSize.y));
      m_EditorCamera.SetViewportSize(m_SceneViewportSize.x, m_SceneViewportSize.y);
      m_ActiveScene->OnViewportResize(static_cast<uint32_t>(m_SceneViewportSize.x), static_cast<uint32_t>(m_SceneViewportSize.y));
    }

    Renderer2D::ResetStats();
    InputProccedure(time);

    m_Framebuffer->Bind();

    if(m_SceneHierarchy.GetContext())
      RenderCommand::ClearColor(clearColor);
    else
      RenderCommand::ClearColor(glm::vec4(0.6f));

    RenderCommand::Clear();
    m_Framebuffer->ClearAttachment(1, -1);


    switch (m_SceneState)
    {
    case SceneState::Play:
      m_GizmosType = -1;
      m_ActiveScene->OnViewportResize(static_cast<uint32_t>(m_SceneViewportSize.x), static_cast<uint32_t>(m_SceneViewportSize.y));
      m_ActiveScene->OnUpdateRuntime(time);
      break;

    case SceneState::Edit:
      m_EditorCamera.OnUpdate(time);
      m_ActiveScene->OnUpdateEditor(time, m_EditorCamera);
      break;

    case SceneState::Simulate:
			m_EditorCamera.OnUpdate(time);
			m_ActiveScene->OnUpdateSimulation(time, m_EditorCamera);
      break;
    }

    auto [mx, my] = ImGui::GetMousePos();
    mx -= m_SceneViewportBounds[0].x;
    my -= m_SceneViewportBounds[0].y;
    const glm::vec2 viewportSize = m_SceneViewportBounds[1] - m_SceneViewportBounds[0];
    my = viewportSize.y - my;
    mouseX = static_cast<int>(mx);
    mouseY = static_cast<int>(my) - 1;

    if (mouseX >= 0 && mouseY >= 0 && mouseX < static_cast<int>(viewportSize.x) && mouseY < static_cast<int>(viewportSize.y))
    {
      m_PixelData = m_Framebuffer->ReadPixel(1, mouseX, mouseY);
      m_HoveredEntity = m_PixelData == -1 ? Entity() : Entity(static_cast<entt::entity>(m_PixelData), m_ActiveScene.get());
    }

    OnOverlayRenderer();
    m_Framebuffer->Unbind();

    {
      // Game Framebuffer
			// Resize
      const FramebufferSpecification gameSpec = m_GameFramebuffer->GetSpecification();
			if (m_GameViewportSize.x > 0.0f && m_GameViewportSize.y > 0.0f && (m_GameViewportSize.x != gameSpec.Width || m_GameViewportSize.y != gameSpec.Height))
        m_GameFramebuffer->Resize(static_cast<uint32_t>(m_GameViewportSize.x), static_cast<uint32_t>(m_GameViewportSize.y));

			m_GameFramebuffer->Bind();
			RenderCommand::ClearColor(clearColor);

			RenderCommand::Clear();
			m_GameFramebuffer->ClearAttachment(0, -1);

      m_ActiveScene->OnViewportResize(static_cast<uint32_t>(m_GameViewportSize.x), static_cast<uint32_t>(m_GameViewportSize.y));
      m_ActiveScene->OnUpdateGame(time);

			m_GameFramebuffer->Unbind();
    }
  }

	void Editor::OnDuplicateEntity()
	{
		if (m_SceneState != SceneState::Edit)
			return;

		Entity selectedEntity = m_SceneHierarchy.GetSelectedEntity();
    if (selectedEntity)
			m_EditorScene->DuplicateEntity(selectedEntity);
	}

	void Editor::OnGuiRender()
  {
    m_Dockspace.Begin();
    MenuBar();

    GameViewport();
    SceneViewport();

    m_SceneHierarchy.OnImGuiRender();
    m_ContentBrowser.OnImGuiRender();

    m_Dockspace.End();
  }

	void Editor::OverlayBeginScene()
	{
		if (m_SceneState == SceneState::Play)
		{
			Entity& camera = m_ActiveScene->GetPrimaryCameraEntity();
			if (camera)
			{
				glm::mat4 transform = camera.GetComponent<TransformComponent>().GetTransform();
				Renderer2D::BeginScene(camera.GetComponent<CameraComponent>().Camera, transform);
			}
		}
		else Renderer2D::BeginScene(m_EditorCamera);
	}

	void Editor::OnOverlayRenderer()
	{
		float zIndex = 0.001f;
		glm::vec3 ccFWDir = m_EditorCamera.GetForwardDirection();
		glm::vec3 projectionRender = ccFWDir * glm::vec3(zIndex);

    OverlayBeginScene();

    if (m_VisualizeCollider)
    {
			// Circle Collider Visualizer
			{
				auto& view = m_ActiveScene->GetAllEntitiesWith<TransformComponent, CircleCollider2DComponent>();
				for (auto entity : view)
				{
					auto& [tc, cc2d] = view.get<TransformComponent, CircleCollider2DComponent>(entity);

					glm::vec3 translation = tc.Translation + glm::vec3(cc2d.Offset, -projectionRender.z);
					glm::vec3 scale = tc.Scale * glm::vec3(cc2d.Radius * 2.0f);

					glm::mat4 transform = glm::translate(glm::mat4(1.0f), translation)
						* glm::scale(glm::mat4(1.0f), scale);

					Renderer2D::DrawCircle(transform, glm::vec4(0, 1, 0, 1), 0.05f);
				}
			}

			// Quad Collider Visualizer
			{
				auto& view = m_ActiveScene->GetAllEntitiesWith<TransformComponent, BoxCollider2DComponent>();
				for (auto entity : view)
				{
					auto& [tc, bc2d] = view.get<TransformComponent, BoxCollider2DComponent>(entity);

					glm::vec3 scale = tc.Scale * glm::vec3(bc2d.Size * 2.0f, 1.0f);

					glm::mat4 transform = glm::translate(glm::mat4(1.0f), tc.Translation)
						* glm::rotate(glm::mat4(1.0f), tc.Rotation.z, glm::vec3(0.0f, 0.0f, 1.0f))
						* glm::translate(glm::mat4(1.0f), glm::vec3(bc2d.Offset, projectionRender.z))
						* glm::scale(glm::mat4(1.0f), scale);

					Renderer2D::DrawRect(transform, glm::vec4(0, 1, 0, 1));
				}
			}
    }
    RenderCommand::SetLineWidth(2.3f);
    Renderer2D::EndScene();

		if (Entity selectedEntity = m_SceneHierarchy.GetSelectedEntity())
    {
			const auto& tc = selectedEntity.GetComponent<TransformComponent>();
      glm::mat4 rotation = glm::toMat4(glm::quat(tc.Rotation));

      if (selectedEntity.HasComponent<CircleRendererComponent>())
      {
				glm::vec3 translation = tc.Translation + glm::vec3(0.0f, 0.0f, -projectionRender.z);
				glm::vec3 scale = tc.Scale * glm::vec3(1.0f);

				glm::mat4 transform = glm::translate(glm::mat4(1.0f), translation)
					* rotation * glm::scale(glm::mat4(1.0f), scale);

				Renderer2D::DrawCircle(transform, glm::vec4(1.0f, 0.5f, 0.0f, 1.0f), 0.05f);
      }
      else
      {
        glm::vec3 translation = tc.Translation + glm::vec3(0.0f, 0.0f, -projectionRender.z);
				glm::mat4 transform = glm::translate(glm::mat4(1.0f), translation)
          * rotation * glm::scale(glm::mat4(1.0f), tc.Scale);

				Renderer2D::DrawRect(transform, glm::vec4(1.0f, 0.5f, 0.0f, 1.0f));
      }
		}

    RenderCommand::SetLineWidth(2.0f);
    Renderer2D::EndScene();

    RenderCommand::SetLineWidth(1.0f);
	}

  void Editor::OnScenePlay()
  {
    if (m_SceneState == SceneState::Simulate)
      OnSceneStop();

    m_SceneState = SceneState::Play;

    m_ActiveScene = Scene::Copy(m_EditorScene);
    m_ActiveScene->OnRuntimeStart();

    m_SceneHierarchy.SetContext(m_ActiveScene);
  }

	void Editor::OnSceneSimulate()
	{
		if (m_SceneState == SceneState::Play)
			OnSceneStop();

		m_SceneState = SceneState::Simulate;

		m_ActiveScene = Scene::Copy(m_EditorScene);
		m_ActiveScene->OnSimulationStart();

		m_SceneHierarchy.SetContext(m_ActiveScene);
	}

	void Editor::OnSceneStop()
  {
    OGN_CORE_ASSERT(m_SceneState == SceneState::Play || m_SceneState == SceneState::Simulate, "");

    m_GizmosType = -1;

    if (m_SceneState == SceneState::Play)
      m_ActiveScene->OnRuntimeStop();
    else if(m_SceneState == SceneState::Simulate)
      m_ActiveScene->OnSimulationStop();

    m_SceneState = SceneState::Edit;
    m_ActiveScene = m_EditorScene;

    m_SceneHierarchy.SetContext(m_ActiveScene);
  }

	void Editor::NewScene()
  {
		if (m_SceneState == SceneState::Play)
			OnSceneStop();

		m_GizmosType = -1;

    m_HoveredEntity = {};
    m_SceneHierarchy.SetSelectedEntity({});

		m_EditorScene = std::make_shared<Scene>();
		m_EditorScene->OnViewportResize(static_cast<uint32_t>(m_SceneViewportSize.x), static_cast<uint32_t>(m_SceneViewportSize.y));
		m_EditorCamera.SetViewportSize(m_SceneViewportSize.x, m_SceneViewportSize.y);

		m_SceneHierarchy.SetContext(m_EditorScene);
    m_ActiveScene = m_EditorScene;

    m_ScenePath = std::filesystem::path();
  }

	void Editor::SaveScene()
	{
    if (!m_ScenePath.empty())
      SerializeScene(m_ActiveScene, m_ScenePath);
    else SaveSceneAs();
	}

	void Editor::SaveSceneAs()
  {
    std::filesystem::path filepath = FileDialogs::SaveFile("ORigin Scene (*.org,*.origin)\0*.org\0");

    if (!filepath.empty())
    {
      SerializeScene(m_ActiveScene, m_ScenePath);
      m_ScenePath = filepath;
    }
  }

  void Editor::OpenScene(const std::filesystem::path& path)
  {
		if (m_SceneState == SceneState::Play)
			OnSceneStop();

    m_GizmosType = -1;

    m_HoveredEntity = {};
    m_SceneHierarchy.SetSelectedEntity({});

    std::shared_ptr<Scene> newScene = std::make_shared<Scene>();
    SceneSerializer serializer(newScene);
    if (serializer.Deserialize(path.string()))
    {
      m_EditorScene = newScene;
      m_EditorScene->OnViewportResize(static_cast<uint32_t>(m_SceneViewportSize.x), static_cast<uint32_t>(m_SceneViewportSize.y));
			m_EditorCamera.SetViewportSize(m_SceneViewportSize.x, m_SceneViewportSize.y);
      m_SceneHierarchy.SetContext(m_EditorScene);

      m_ActiveScene = m_EditorScene;
      m_ScenePath = path;
    }
  }

  void Editor::OpenScene()
  {
    if (m_SceneState == SceneState::Play)
      OnSceneStop();

    std::filesystem::path filepath = FileDialogs::OpenFile("ORigin Scene (*.org,*.origin)\0*.org\0");
    if (!filepath.empty()) OpenScene(filepath);
  }

	void Editor::SerializeScene(std::shared_ptr<Scene>& scene, const std::filesystem::path& scenePath)
	{
    SceneSerializer serializer(scene);
    serializer.Serialize(scenePath);
	}
}