// Copyright (c) Evangelion Manuhutu | ORigin Engine

#include "EditorLayer.h"
#include "Origin/EntryPoint.h"
#include "Origin/Utils/PlatformUtils.h"
#include "Origin/Scene/SceneSerializer.h"
#include "Origin/Scripting/ScriptEngine.h"
#include "Origin/Renderer/Font.h"
#include "Origin/Asset/AssetManager.h"

#include "Origin/Audio/Audio.h"
#include "Origin/Utils/Utils.h"

#include "Origin/Asset/TextureImporter.h"
#include "Origin/Asset/SceneImporter.h"

#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <imgui_internal.h>
#include <filesystem>

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/quaternion.hpp>
#include <glm/gtx/compatibility.hpp>

namespace origin {

  EditorLayer* EditorLayer::s_Instance = nullptr;

  EditorLayer::EditorLayer() : Layer("Editor")
  {
    s_Instance = this;
  }

  EditorLayer::~EditorLayer()
  {
    ScriptEngine::Shutdown();
  }

  void EditorLayer::OnAttach()
  {
		m_PlayButton = TextureImporter::LoadTexture2D("Resources/UITextures/playbutton.png");
		m_SimulateButton = TextureImporter::LoadTexture2D("Resources/UITextures/simulatebutton.png");
		m_StopButton = TextureImporter::LoadTexture2D("Resources/UITextures/stopbutton.png");
		m_PauseButton = TextureImporter::LoadTexture2D("Resources/UITextures/pausebutton.png");
		m_SteppingButton = TextureImporter::LoadTexture2D("Resources/UITextures/steppingframebutton.png");

    // ==============================
    // Main Framebuffer Specification
    // ==============================
    FramebufferSpecification mainFramebufferSpec;
    mainFramebufferSpec.Attachments =
    {
        FramebufferTextureFormat::RGBA8,
        FramebufferTextureFormat::RED_INTEGER,
        FramebufferTextureFormat::DEPTH24STENCIL8
    };

    mainFramebufferSpec.Width = 1280;
    mainFramebufferSpec.Height = 720;
    m_Framebuffer = Framebuffer::Create(mainFramebufferSpec);

    // ==============================
    // Game Framebuffer Specification
    // ==============================
    FramebufferSpecification gameFramebufferSpec;
    gameFramebufferSpec.Attachments =
    {
        FramebufferTextureFormat::RGBA8,
        FramebufferTextureFormat::DEPTH24STENCIL8
    };

    gameFramebufferSpec.Width = 1280;
    gameFramebufferSpec.Height = 720;
    gameFramebufferSpec.ReadBuffer = false;
    m_GameFramebuffer = Framebuffer::Create(gameFramebufferSpec);

    m_EditorCamera = EditorCamera(45.0f, 1.778f, 0.1f, 1000.0f);
    m_EditorCamera.SetPosition(glm::vec3(0.0f, 1.0f, 10.0f));
    m_EditorCamera.SetStyle(FreeMove);

    m_ActiveScene = std::make_shared<Scene>();
    const auto commandLineArgs = Application::Get().GetSpecification().CommandLineArgs;
    if (commandLineArgs.Count > 1)
    {
        m_ProjectPath = commandLineArgs[1];
        if (!OpenProject(m_ProjectPath))
          Application::Get().Close();
    }
    else
    {
			if (!OpenProject())
			{
				if (!NewProject())
					Application::Get().Close();
			}
    }
  }

  void EditorLayer::OnEvent(Event& e)
  {
      m_EditorCamera.OnEvent(e);
      EventDispatcher dispatcher(e);
      dispatcher.Dispatch<MouseMovedEvent>(OGN_BIND_EVENT_FN(EditorLayer::OnMouseMovedEvent));
      dispatcher.Dispatch<WindowResizeEvent>(OGN_BIND_EVENT_FN(EditorLayer::OnWindowResize));
      dispatcher.Dispatch<KeyPressedEvent>(OGN_BIND_EVENT_FN(EditorLayer::OnKeyPressed));
      dispatcher.Dispatch<MouseButtonPressedEvent>(OGN_BIND_EVENT_FN(EditorLayer::OnMouseButtonPressed));
  }

  void EditorLayer::OnUpdate(Timestep deltaTime)
  {
    m_Time += deltaTime.Seconds();
    const ImGuiIO& io = ImGui::GetIO();

    const bool enableCamera =
        io.WantTextInput == false
        || !ImGuizmo::IsUsing()
        || Application::Get().GetGuiLayer()->GetActiveWidgetID() == 0;

    m_EditorCamera.EnableMovement(enableCamera);

    Renderer2D::ResetStats();
    Renderer3D::ResetStats();
    InputProcedure(deltaTime);

    // Resize
    if (const FramebufferSpecification spec = m_Framebuffer->GetSpecification();
        m_SceneViewportSize.x > 0.0f && m_SceneViewportSize.y > 0.0f && (m_SceneViewportSize.x != spec.Width ||
            m_SceneViewportSize.y != spec.Height))
    {
        m_Framebuffer->Resize(static_cast<uint32_t>(m_SceneViewportSize.x),
                              static_cast<uint32_t>(m_SceneViewportSize.y));
        m_EditorCamera.SetViewportSize(m_SceneViewportSize.x, m_SceneViewportSize.y);
        m_ActiveScene->OnViewportResize(static_cast<uint32_t>(m_SceneViewportSize.x),
                                        static_cast<uint32_t>(m_SceneViewportSize.y));
    }

    m_Framebuffer->Bind();

    RenderCommand::Clear();
    RenderCommand::ClearColor(clearColor);
      
    m_Framebuffer->ClearAttachment(1, -1);

    switch (m_SceneState)
    {
    case SceneState::Play:
        m_GizmosType = -1;
        m_ActiveScene->OnViewportResize(static_cast<uint32_t>(m_SceneViewportSize.x),
                                        static_cast<uint32_t>(m_SceneViewportSize.y));
        m_ActiveScene->OnUpdateRuntime(deltaTime);
        break;

    case SceneState::Edit:
        m_EditorCamera.OnUpdate(deltaTime);
        m_ActiveScene->OnUpdateEditor(deltaTime, m_EditorCamera);
        OnOverlayRenderer();
        break;

    case SceneState::Simulate:
        m_EditorCamera.OnUpdate(deltaTime);
        m_ActiveScene->OnUpdateSimulation(deltaTime, m_EditorCamera);
        OnOverlayRenderer();
        break;
    }

    auto [mx, my] = ImGui::GetMousePos();
    glm::vec2 mousePos = {mx, my};
    mousePos -= m_SceneViewportBounds[0];
    const glm::vec2& viewportSize = m_SceneViewportBounds[1] - m_SceneViewportBounds[0];
    mousePos.y = viewportSize.y - mousePos.y;
    mousePos = glm::clamp(mousePos, glm::vec2(0.0f), viewportSize - glm::vec2(1.0f));
    mouseX = static_cast<int>(mousePos.x);
    mouseY = static_cast<int>(mousePos.y);

    m_PixelData = m_Framebuffer->ReadPixel(1, mouseX, mouseY);
    m_HoveredEntity = m_PixelData == -1
                          ? Entity()
                          : Entity(static_cast<entt::entity>(m_PixelData), m_ActiveScene.get());

    if(Entity selectedEntity = m_SceneHierarchy.GetSelectedEntity())
        m_EditorCamera.SetEntityObject(selectedEntity);
    else
        m_EditorCamera.SetEntityObject({});

    for (auto& id : m_ActiveScene->m_Registry.view<RigidbodyComponent>())
    {
      Entity entity = { id, m_ActiveScene.get() };
      auto& rb = entity.GetComponent<RigidbodyComponent>();
      glm::vec3 velocity = glm::vec3(0.0f);

      if (Input::IsKeyPressed(Key::I))
      {
        velocity.y += deltaTime * 5.0f;
      }

      rb.AddForce(velocity, RigidbodyComponent::ForceMode::Impulse);
    }
    m_Framebuffer->Unbind();

    m_ActiveScene->OnShadowRender();
  }

  void EditorLayer::OnDuplicateEntity()
  {
      if (m_SceneState != SceneState::Edit)
          return;

      if (Entity selectedEntity = m_SceneHierarchy.GetSelectedEntity())
      {
          Entity entity = m_EditorScene->DuplicateEntity(selectedEntity);
          m_SceneHierarchy.SetSelectedEntity(entity);
      }
  }

  void EditorLayer::OnGuiRender()
  {
      m_Dockspace.Begin();
      MenuBar();
      SceneViewport();
      m_SceneHierarchy.OnImGuiRender();
      m_ContentBrowser->OnImGuiRender();
      GUIRender();
      m_Dockspace.End();
  }

  void EditorLayer::OverlayBeginScene()
  {
      if (m_SceneState == SceneState::Play)
      {
          if (Entity camera = m_ActiveScene->GetPrimaryCameraEntity())
              Renderer::BeginScene(camera.GetComponent<CameraComponent>().Camera, camera.GetComponent<TransformComponent>());
      }
      else Renderer::BeginScene(m_EditorCamera);
  }

  void EditorLayer::OnOverlayRenderer()
  {
      float zIndex = 0.001f;
      glm::vec3 ccFWDir = m_EditorCamera.GetForwardDirection();
      glm::vec3 projectionRender = ccFWDir * glm::vec3(zIndex);

      OverlayBeginScene();

      if (m_VisualizeCollider)
      {
        // Circle Collider Visualizer
				const auto& circle = m_ActiveScene->GetAllEntitiesWith<TransformComponent, CircleCollider2DComponent>();
				for (auto entity : circle)
				{
					const auto& [tc, cc2d] = circle.get<TransformComponent, CircleCollider2DComponent>(entity);

					glm::vec3 translation = tc.Translation + glm::vec3(cc2d.Offset, -projectionRender.z);
					glm::vec3 scale = tc.Scale * glm::vec3(cc2d.Radius * 2.0f);

					glm::mat4 transform = glm::translate(glm::mat4(1.0f), translation)
						* glm::scale(glm::mat4(1.0f), scale);

          
					Renderer2D::DrawCircle(transform, glm::vec4(0, 1, 0, 1), 0.05f);
				}

        // Quad Collider Visualizer
				const auto& quad = m_ActiveScene->GetAllEntitiesWith<TransformComponent, BoxCollider2DComponent>();
				for (auto entity : quad)
				{
					const auto& [tc, bc2d] = quad.get<TransformComponent, BoxCollider2DComponent>(entity);

          glm::vec3 pos = glm::vec3(glm::vec2(tc.Translation) + bc2d.Offset, projectionRender.z);
					glm::vec3 scale = tc.Scale * glm::vec3(bc2d.Size * 2.0f, 1.0f);

					glm::mat4 transform = glm::translate(glm::mat4(1.0f), pos)
						* glm::rotate(glm::mat4(1.0f), tc.Rotation.z, glm::vec3(0.0f, 0.0f, 1.0f))
						* glm::scale(glm::mat4(1.0f), scale);

					Renderer2D::DrawRect(transform, glm::vec4(0.0f, 1.0f, 0.0f, 1.0f));
				}

        float lineWidth = 0;
				const auto& box = m_ActiveScene->GetAllEntitiesWith<TransformComponent, BoxColliderComponent>();
				for (auto entity : box)
				{
					const auto& [tc, bc] = box.get<TransformComponent, BoxColliderComponent>(entity);

          glm::vec3 scale = tc.Scale * glm::vec3(bc.Size * 2.0f);

					glm::mat4 transform = glm::translate(glm::mat4(1.0f), glm::vec3(tc.Translation + bc.Offset))
						* glm::toMat4(glm::quat(tc.Rotation))
						* glm::scale(glm::mat4(1.0f), scale * 2.0f);

					lineWidth = glm::clamp(glm::length(tc.Translation + m_EditorCamera.GetPosition()), 2.5f, 3.0f);
					Renderer3D::DrawRect(transform, glm::vec4(1.0f, 0.0f, 1.0f, 1.0f));
				}
        RenderCommand::SetLineWidth(lineWidth);
        Renderer2D::EndScene();

				const auto& sphere = m_ActiveScene->GetAllEntitiesWith<TransformComponent, SphereColliderComponent>();
				for (auto entity : sphere)
				{
					const auto& [tc, cc] = sphere.get<TransformComponent, SphereColliderComponent>(entity);
          glm::vec3 scale = tc.Scale * glm::vec3(cc.Radius * 2.0f);

					lineWidth = glm::clamp(glm::length(tc.Translation + m_EditorCamera.GetPosition()), 0.005f, 0.05f);
					glm::mat4 transform = glm::translate(glm::mat4(1.0f), glm::vec3(tc.Translation + cc.Offset))
						* glm::rotate(glm::mat4(1.0f), tc.Rotation.x, glm::vec3(1, 0, 0))
						* glm::rotate(glm::mat4(1.0f), 0.0f, glm::vec3(0, 1, 0))
						* glm::rotate(glm::mat4(1.0f), tc.Rotation.z, glm::vec3(0, 0, 1))
						* glm::scale(glm::mat4(1.0f), glm::vec3(scale * 2.1f));
					Renderer2D::DrawCircle(transform, glm::vec4(0.7f, 0.0f, 1.0f, 1.0f), lineWidth);

					transform = glm::translate(glm::mat4(1.0f), glm::vec3(tc.Translation + cc.Offset))
						* glm::rotate(glm::mat4(1.0f), tc.Rotation.x + glm::radians(90.0f), glm::vec3(1, 0, 0))
						* glm::rotate(glm::mat4(1.0f), tc.Rotation.z, glm::vec3(0, 1, 0))
						* glm::rotate(glm::mat4(1.0f), 0.0f, glm::vec3(0, 0, 1))
						* glm::scale(glm::mat4(1.0f), glm::vec3(scale * 2.1f));
					Renderer2D::DrawCircle(transform, glm::vec4(0.7f, 0.0f, 1.0f, 1.0f), lineWidth);
				}
        RenderCommand::SetLineWidth(lineWidth);
        Renderer2D::EndScene();
      }
     

      // Visualizing Selecting Entity
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

          if (selectedEntity.HasComponent<SpriteRenderer2DComponent>())
          {
              glm::vec3 translation = tc.Translation + glm::vec3(0.0f, 0.0f, -projectionRender.z);
              glm::mat4 transform = glm::translate(glm::mat4(1.0f), translation)
                  * rotation * glm::scale(glm::mat4(1.0f), tc.Scale);

              Renderer2D::DrawRect(transform, glm::vec4(1.0f, 0.5f, 0.0f, 1.0f));
          }
          if (selectedEntity.HasComponent<SpriteRendererComponent>())
          {
              glm::vec3 translation = tc.Translation + glm::vec3(0.0f, 0.0f, -projectionRender.z);
              glm::mat4 transform = glm::translate(glm::mat4(1.0f), translation)
                  * rotation * glm::scale(glm::mat4(1.0f), tc.Scale);

              Renderer3D::DrawRect(transform, glm::vec4(1.0f, 0.5f, 0.0f, 1.0f));
          }
      }
      RenderCommand::SetLineWidth(3.0f);
      Renderer2D::EndScene();

      RenderCommand::SetLineWidth(1.0f);
  }

  void EditorLayer::OnScenePlay()
  {
      if (m_SceneState == SceneState::Simulate)
          OnSceneStop();

      m_SceneState = SceneState::Play;

      m_ActiveScene = Scene::Copy(m_EditorScene);
      m_ActiveScene->OnRuntimeStart();

      m_SceneHierarchy.SetContext(m_ActiveScene);
  }

  void EditorLayer::OnScenePause()
  {
      if (m_SceneState == SceneState::Edit)
          return;

      m_ActiveScene->SetPaused(true);
  }

  void EditorLayer::OnSceneSimulate()
  {
      if (m_SceneState == SceneState::Play)
          OnSceneStop();

      m_SceneState = SceneState::Simulate;

      m_ActiveScene = Scene::Copy(m_EditorScene);
      m_ActiveScene->OnSimulationStart();

      m_SceneHierarchy.SetContext(m_ActiveScene);
  }

  void EditorLayer::OnSceneStop()
  {
    if (m_SceneState == SceneState::Play)
        m_ActiveScene->OnRuntimeStop();
    else if (m_SceneState == SceneState::Simulate)
        m_ActiveScene->OnSimulationStop();

    m_SceneHierarchy.SetContext(m_EditorScene);
    m_ActiveScene = m_EditorScene;

    m_SceneState = SceneState::Edit;
  }

  bool EditorLayer::NewProject()
  {
      Project::New();

			AssetHandle handle = Project::GetActive()->GetConfig().StartScene;
			if (handle)
				OpenScene(handle);

			ScriptEngine::Init();
			m_ProjectPath = Project::GetActiveProjectDirectory();

			m_ContentBrowser = std::make_unique<ContentBrowserPanel>(Project::GetActive());
			if (!m_ContentBrowser)
			{
				OGN_CORE_ERROR("Editor Layer: ContentBrowserPanel Failed to initialized");
				return false;
			}

			return true;
  }

  bool EditorLayer::OpenProject(const std::filesystem::path& path)
  {
    if (Project::Load(path))
    {
      AssetHandle handle = Project::GetActive()->GetConfig().StartScene;
      if (handle)
				OpenScene(handle);

      ScriptEngine::Init();
			m_ProjectPath = Project::GetActiveProjectDirectory();

      m_ContentBrowser = std::make_unique<ContentBrowserPanel>(Project::GetActive());
			if (!m_ContentBrowser)
			{
				OGN_CORE_ERROR("Editor Layer: ContentBrowserPanel Failed to initialized");
				return false;
			}

      return true;
    }

    return false;
  }

  bool EditorLayer::OpenProject()
  {
    if (Project::Open())
    {
      ScriptEngine::Init();

      AssetHandle handle = Project::GetActive()->GetConfig().StartScene;
      if (handle)
				OpenScene(handle);
      
      m_ProjectPath = Project::GetActiveProjectDirectory();

      m_ContentBrowser = std::make_unique<ContentBrowserPanel>(Project::GetActive());
			if (!m_ContentBrowser)
			{
				OGN_CORE_ERROR("Editor Layer: ContentBrowserPanel Failed to initialized");
				return false;
			}

      return true;
    }

    return false;
  }

  void EditorLayer::SaveProject()
  {
    Project::SaveActive(m_ProjectPath);
  }

  void EditorLayer::NewScene()
  {
    if (m_SceneState == SceneState::Play || m_SceneState == SceneState::Simulate)
      OnSceneStop();

    m_EditorScene = std::make_shared<Scene>();
    m_SceneHierarchy.SetContext(m_EditorScene, true);

    m_ActiveScene = m_EditorScene;

    m_ScenePath = std::filesystem::path();

		// If there is no scene
		if (Project::GetActive()->GetConfig().StartScene == 0)
			Project::GetActive()->SetStartScene(m_ScenePath);
  }

  void EditorLayer::SaveScene()
  {
    if (m_SceneState == SceneState::Play || m_SceneState == SceneState::Simulate)
        OnSceneStop();

    if (!m_ScenePath.empty())
      SerializeScene(m_ActiveScene, m_ScenePath);
    else
        SaveSceneAs();
  }

  void EditorLayer::SaveSceneAs()
  {
    if (m_SceneState == SceneState::Play || m_SceneState == SceneState::Simulate)
        OnSceneStop();

    std::filesystem::path filepath = FileDialogs::SaveFile("ORigin Scene (*.org,*.origin)\0*.org\0");

    if (!filepath.empty())
    {
      SerializeScene(m_ActiveScene, filepath);
      m_ScenePath = filepath;
    }
  }

  void EditorLayer::OpenScene(AssetHandle handle)
  {
    OGN_CORE_ASSERT(handle);

    if (m_SceneState != SceneState::Edit)
      OnSceneStop();

    std::shared_ptr<Scene> readOnlyScene = AssetManager::GetAsset<Scene>(handle);
    std::shared_ptr<Scene> newScene = Scene::Copy(readOnlyScene);
		m_HoveredEntity = {};

		m_EditorScene = newScene;
		m_SceneHierarchy.SetContext(m_EditorScene, true);
		m_ActiveScene = m_EditorScene;
    m_ScenePath = Project::GetActive()->GetEditorAssetManager()->GetFilepath(handle);
  }

  void EditorLayer::OpenScene()
  {
      if (m_SceneState == SceneState::Play)
        OnSceneStop();

      std::filesystem::path filepath = FileDialogs::OpenFile("ORigin Scene (*.org,*.origin)\0*.org\0");
			AssetHandle handle = SceneImporter::OpenScene(filepath);
			if (handle == 0 || filepath.empty())
				return;

			std::shared_ptr<Scene> readOnlyScene = AssetManager::GetAsset<Scene>(handle);
			std::shared_ptr<Scene> newScene = Scene::Copy(readOnlyScene);
			m_HoveredEntity = {};

			m_EditorScene = newScene;
			m_SceneHierarchy.SetContext(m_EditorScene, true);
			m_ActiveScene = m_EditorScene;
			m_ScenePath = Project::GetActive()->GetEditorAssetManager()->GetFilepath(handle);
  }

  void EditorLayer::SerializeScene(std::shared_ptr<Scene>& scene, const std::filesystem::path& scenePath)
  {
    SceneImporter::SaveScene(scene, scenePath);
  }

	void EditorLayer::MenuBar()
	{
		ImGuiStyle& style = ImGui::GetStyle();
		auto& window = Application::Get();
		ImGuiIO& io = ImGui::GetIO();

		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(8, 8));

		if (ImGui::BeginMenuBar())
		{
			ImGui::PushStyleColor(ImGuiCol_MenuBarBg, ImVec4(1.0f, 1.0f, 1.0f, 1.0f));

			if (ImGui::BeginMenu("File"))
			{
				if (ImGui::MenuItem("New Project")) NewProject();
				if (ImGui::MenuItem("Open Project")) OpenProject();
				ImGui::Separator();
				if (ImGui::MenuItem("New Scene", "Ctrl+N")) NewScene();
				if (ImGui::MenuItem("Open Scene", "Ctrl+O"))  OpenScene();
				if (ImGui::MenuItem("Save Scene", "Ctrl+S"))  SaveScene();
				if (ImGui::MenuItem("Save Scene As", "Ctrl+Shift+S"))  SaveSceneAs();
				if (ImGui::MenuItem("Exit", "Alt+F4")) window.Close();
				ImGui::EndMenu();
			}

			if (ImGui::BeginMenu("Script"))
			{
				if (ImGui::MenuItem("Reload"))
				{
					if (m_SceneState == SceneState::Play || m_SceneState == SceneState::Simulate)
						OnSceneStop();

					ScriptEngine::ReloadAssembly();
				}
				ImGui::EndMenu();
			}

			if (ImGui::BeginMenu("Object"))
			{
				ImGui::MenuItem("Audio Library", nullptr, &guiAudioLibraryWindow);
				ImGui::EndMenu();
			}

			if (ImGui::BeginMenu("Window"))
			{
				ImGui::MenuItem("Style Editor", nullptr, &guiMenuStyle);
				ImGui::MenuItem("Render Status", nullptr, &guiRenderStatusWindow);
				ImGui::MenuItem("Debug Info", nullptr, &guiDebugInfo);
				ImGui::MenuItem("Demo Window", nullptr, &guiImGuiDemoWindow);

				ImGui::EndMenu();
			}

			if (ImGui::BeginMenu("View"))
			{
				if (ImGui::MenuItem("Fullscreen", "F11", &guiMenuFullscreen))
					window.GetWindow().SetFullscreen(guiMenuFullscreen);
				ImGui::EndMenu();
			}

			ImGui::EndMenuBar();
			ImGui::PopStyleColor();
			ImGui::PopStyleVar();
		}
	}

	void EditorLayer::SceneViewport()
	{
		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));

		ImGuiWindowFlags window_flags = ImGuiWindowFlags_NoScrollbar
			| ImGuiWindowFlags_NoScrollWithMouse
			| ImGuiWindowFlags_NoCollapse;

		ImGui::Begin("Scene", nullptr, window_flags);

		m_SceneViewportHovered = ImGui::IsWindowHovered();
		m_SceneViewportFocused = ImGui::IsWindowFocused();

		Application::Get().GetGuiLayer()->BlockEvents(!m_SceneViewportHovered);

		m_SceneHierarchy.SetHierarchyMenuActive(!ImGui::IsWindowFocused());

		SceneViewportMenu();
		SceneViewportToolbar();

		const ImVec2& viewportMinRegion = ImGui::GetWindowContentRegionMin();
		const ImVec2& viewportMaxRegion = ImGui::GetWindowContentRegionMax();
		const ImVec2& viewportOffset = ImGui::GetWindowPos();
		m_SceneViewportBounds[0] = { viewportMinRegion.x + viewportOffset.x, viewportMinRegion.y + viewportOffset.y };
		m_SceneViewportBounds[1] = { viewportMaxRegion.x + viewportOffset.x, viewportMaxRegion.y + viewportOffset.y };

		// Debug Info Overlay
		if (guiOverlay)
		{
			ImGuiWindowFlags window_flags = ImGuiWindowFlags_NoDecoration
				| ImGuiWindowFlags_AlwaysAutoResize
				| ImGuiWindowFlags_NoSavedSettings
				| ImGuiWindowFlags_NoFocusOnAppearing
				| ImGuiWindowFlags_NoNav;

			ImGui::SetNextWindowPos(
				{ (viewportMinRegion.x + viewportOffset.x) + 8.0f,
				(viewportMinRegion.y + viewportOffset.y) + 8.0f }, ImGuiCond_Always);

			ImGui::SetNextWindowBgAlpha(0.0f); // Transparent background
			if (ImGui::Begin("##top_left_overlay", nullptr, window_flags))
			{
				if (!m_SceneHierarchy.GetContext())
					ImGui::Text("Load a Scene or Create New Scene to begin!");

				ImGui::Text("%.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
			}

			ImGui::End();
		}

		ImVec2& viewportPanelSize = ImGui::GetContentRegionAvail();
		m_SceneViewportSize = { viewportPanelSize.x, viewportPanelSize.y };

		auto viewportID = (ImTextureID)m_Framebuffer->GetColorAttachmentRendererID(m_RenderTarget);
		ImGui::Image(viewportID, ImVec2(m_SceneViewportSize.x, m_SceneViewportSize.y), ImVec2(0, 1), ImVec2(1, 0));
		if (ImGui::BeginDragDropTarget())
		{

			// =============================
			// ==== Scene Drag and Drop ====
			// =============================

			if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("CONTENT_BROWSER_ITEM"))
			{
				if (m_SceneState == SceneState::Edit)
				{
					if (m_HoveredEntity)
					{
						m_SceneHierarchy.SetSelectedEntity(m_HoveredEntity);
						auto entity = m_SceneHierarchy.GetSelectedEntity();
						if (entity.HasComponent<SpriteRenderer2DComponent>())
						{
							const wchar_t* path = (const wchar_t*)payload->Data;
							std::filesystem::path textureFile = Project::GetActiveAssetFileSystemPath(path);
							auto& component = entity.GetComponent<SpriteRenderer2DComponent>();
							if (textureFile.extension() == ".png" || textureFile.extension() == ".jpg")
							{
								std::shared_ptr<Texture2D>& texture = AssetManager::GetAsset<Texture2D>(component.Texture);
								texture = TextureImporter::LoadTexture2D(textureFile.string());
							}
						}

						if (entity.HasComponent<SpriteRendererComponent>())
						{
							const wchar_t* path = (const wchar_t*)payload->Data;
							std::filesystem::path textureFile = Project::GetActiveAssetFileSystemPath(path);
							auto& component = entity.GetComponent<SpriteRendererComponent>();
							if (textureFile.extension() == ".png" || textureFile.extension() == ".jpg")
								component.Texture = Texture2D::Create(textureFile.string());
						}
					}
					else
					{
						AssetHandle handle = *(AssetHandle*)payload->Data;
						OpenScene(handle);
					}
				}
			}
			ImGui::EndDragDropTarget();
		}

		// Gizmos
		ImGuizmo::SetDrawlist();
		ImGuizmo::SetRect(
			m_SceneViewportBounds[0].x, m_SceneViewportBounds[0].y,
			m_SceneViewportBounds[1].x - m_SceneViewportBounds[0].x,
			m_SceneViewportBounds[1].y - m_SceneViewportBounds[0].y
		);

		m_SelectedEntity = m_SceneHierarchy.GetSelectedEntity();
		if (m_SelectedEntity && m_GizmosType != -1) {
			// Editor Camera
			const glm::mat4& cameraProjection = m_EditorCamera.GetProjection();
			glm::mat4 cameraView = m_EditorCamera.GetViewMatrix();

			auto& tc = m_SelectedEntity.GetComponent<TransformComponent>();
			glm::mat4 transform = tc.GetTransform();
			glm::vec3 originalRotation = tc.Rotation;

			bool snap = Input::IsKeyPressed(Key::LeftShift);
			float snapValue = 0.5f;

			if (snap && m_GizmosType == ImGuizmo::OPERATION::ROTATE)
				snapValue = 45.0f;

			float snapValues[3] = { snapValue, snapValue, snapValue };

			ImGuizmo::Manipulate(
				glm::value_ptr(cameraView),
				glm::value_ptr(cameraProjection),
				static_cast<ImGuizmo::OPERATION>(m_GizmosType),
				static_cast<ImGuizmo::MODE>(m_GizmosMode),
				glm::value_ptr(transform),
				nullptr,
				snap ? snapValues : nullptr
			);

			if (ImGuizmo::IsUsing())
			{
				glm::vec3 translation, rotation, scale;
				Math::DecomposeTransformEuler(transform, translation, rotation, scale);

				tc.Translation = translation;
				glm::vec3 deltaRotation = rotation - tc.Rotation;
				tc.Rotation += deltaRotation;
				tc.Scale = scale;
			}
		}

		if (ImGui::IsWindowFocused() && Input::IsKeyPressed(Key::Escape))
			m_GizmosType = -1;

		ImGuiIO& io = ImGui::GetIO();

		ImGui::End();
		ImGui::PopStyleVar();
	}

	void EditorLayer::SceneViewportToolbar()
	{
		ImVec2& viewportMinRegion = ImGui::GetWindowContentRegionMin();
		ImVec2& viewportOffset = ImGui::GetWindowPos();

		const float wndWidth = ImGui::GetWindowWidth();

		ImGuiWindowFlags window_flags = ImGuiWindowFlags_NoScrollbar
			| ImGuiWindowFlags_NoScrollWithMouse
			| ImGuiWindowFlags_AlwaysAutoResize
			| ImGuiWindowFlags_NoDecoration
			| ImGuiWindowFlags_NoCollapse;

		float wndYpos = { (viewportMinRegion.y + viewportOffset.y) + 4.0f };

		// Play Button
		ImGui::SetNextWindowPos({ (viewportMinRegion.x + viewportOffset.x) + wndWidth / 2.5f, wndYpos }, ImGuiCond_Always);
		ImGui::SetNextWindowBgAlpha(0.0f);

		if (ImGui::Begin("##play_button", nullptr, window_flags))
		{
			ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 2.0f));
			ImGui::PushStyleVar(ImGuiStyleVar_ItemInnerSpacing, ImVec2(0.0f, 0.0f));

			{
				std::shared_ptr<Texture2D> icon = (m_SceneState == SceneState::Edit || m_SceneState == SceneState::Simulate) ? m_PlayButton : m_StopButton;
				ImGui::SameLine();
				ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0, 0, 0, 0));
				ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.3f, 0.3f, 0.3f, 0.3f));
				ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0, 0, 0, 0));
				if (ImGui::ImageButton((ImTextureID)icon->GetRendererID(), ImVec2(25.0f, 25.0f)))
				{
					if (m_SceneHierarchy.GetContext())
					{
						if (m_SceneState == SceneState::Edit || m_SceneState == SceneState::Simulate)
						{
							OnScenePlay();
						}
						else if (m_SceneState == SceneState::Play)
						{
							OnSceneStop();
						}
					}
				}
				ImGui::PopStyleColor(3);
			}

			// Simulate Button
			{
				std::shared_ptr<Texture2D> icon = (m_SceneState == SceneState::Edit || m_SceneState == SceneState::Play) ? m_SimulateButton : m_StopButton;
				ImGui::SameLine();
				ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0, 0, 0, 0));
				ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.3f, 0.3f, 0.3f, 0.3f));
				ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0, 0, 0, 0));
				if (ImGui::ImageButton((ImTextureID)icon->GetRendererID(), ImVec2(25.0f, 25.0f)))
				{
					if (m_SceneHierarchy.GetContext())
					{
						if (m_SceneState == SceneState::Edit || m_SceneState == SceneState::Play)
						{
							OnSceneSimulate();
						}
						else if (m_SceneState == SceneState::Simulate)
						{
							OnSceneStop();
						}
					}
				}
				ImGui::PopStyleColor(3);
			}

			// Pause Button
			if (m_SceneState != SceneState::Edit)
			{
				bool isPaused = m_ActiveScene->IsPaused();
				std::shared_ptr<Texture2D> icon = m_PauseButton;
				ImGui::SameLine();
				ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0, 0, 0, 0));
				ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.3f, 0.3f, 0.3f, 0.3f));
				ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0, 0, 0, 0));

				if (ImGui::ImageButton((ImTextureID)icon->GetRendererID(), ImVec2(25.0f, 25.0f)))
				{
					m_ActiveScene->SetPaused(!isPaused);
				}

				ImGui::PopStyleColor(3);

				if (isPaused)
				{
					std::shared_ptr<Texture2D> icon = m_SteppingButton;
					ImGui::SameLine();
					ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0, 0, 0, 0));
					ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.3f, 0.3f, 0.3f, 0.3f));
					ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0, 0, 0, 0));

					if (ImGui::ImageButton((ImTextureID)icon->GetRendererID(), ImVec2(25.0f, 25.0f)))
					{
						m_ActiveScene->Step(12);
					}

					ImGui::PopStyleColor(3);
				}
			}

			ImGui::PopStyleVar(2);
			ImGui::End(); // !viewport_toolbar
		}

		// Guizmo MODE
		ImGui::SetNextWindowPos({ (viewportMinRegion.x + viewportOffset.x) + wndWidth - 120.0f, wndYpos }, ImGuiCond_Always);
		ImGui::SetNextWindowBgAlpha(0.0f);
		if (ImGui::Begin("##gizmo_mode", nullptr, window_flags))
		{
			const ImVec4 btActive = ImVec4(0.2f, 0.2f, 0.2f, 1.0f);
			ImVec4 btLOCAL, btGLOBAL;
			if (m_GizmosMode == ImGuizmo::MODE::LOCAL)
			{
				btGLOBAL = ImVec4(0.8f, 0.1f, 0.1f, 1.0f);
				btLOCAL = btActive;
			}

			if (m_GizmosMode == ImGuizmo::MODE::WORLD)
			{
				btLOCAL = ImVec4(0.1f, 0.2f, 0.8f, 1.0f);
				btGLOBAL = btActive;
			}

			ImGui::PushStyleColor(ImGuiCol_Button, btLOCAL);
			ImGui::PushStyleColor(ImGuiCol_ButtonHovered, btLOCAL);
			ImGui::PushStyleColor(ImGuiCol_ButtonActive, btActive);
			if (ImGui::Button("LOCAL", { 50.0f, 25.0f }))
				m_GizmosMode = ImGuizmo::MODE::LOCAL;
			ImGui::PopStyleColor(3);
			ImGui::SameLine(0.0f, 5.0f);

			ImGui::PushStyleColor(ImGuiCol_Button, btGLOBAL);
			ImGui::PushStyleColor(ImGuiCol_ButtonHovered, btGLOBAL);
			ImGui::PushStyleColor(ImGuiCol_ButtonActive, btActive);
			if (ImGui::Button("GLOBAL", { 50.0f, 25.0f }))
				m_GizmosMode = ImGuizmo::MODE::WORLD;
			ImGui::PopStyleColor(3);

			ImGui::End(); // !gizmo_mode
		}
	}

	void EditorLayer::SceneViewportMenu()
	{
		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(8, 8));

		if (VpMenuContextActive && m_SceneHierarchy.GetContext())
		{
			if (ImGui::BeginPopupContextWindow(nullptr, 1, false))
			{
				// Create Menu
				if (m_VpMenuContext == ViewportMenuContext::CreateMenu)
				{
					ImGui::Text("Add"); ImGui::Separator();

					if (ImGui::MenuItem("Empty"))
					{
						Entity entity = m_SceneHierarchy.GetContext()->CreateEntity("Empty");
						m_SceneHierarchy.SetSelectedEntity(entity);
					}

					if (ImGui::MenuItem("Main Camera"))
					{
						Entity entity = m_SceneHierarchy.GetContext()->CreateCamera("Main Camera");
						entity.AddComponent<AudioListenerComponent>();

						m_SceneHierarchy.SetSelectedEntity(entity);
					}

					if (ImGui::MenuItem("Camera"))
					{
						Entity entity = m_SceneHierarchy.GetContext()->CreateCamera("Camera");
						m_SceneHierarchy.SetSelectedEntity(entity);
					}
					ImGui::Separator();

					ImGui::Text("2D"); ImGui::Separator();
					if (ImGui::MenuItem("Sprite"))
					{
						Entity entity = m_SceneHierarchy.GetContext()->CreateSpriteEntity("Sprite");
						m_SceneHierarchy.SetSelectedEntity(entity);
					}
					if (ImGui::MenuItem("Circle"))
					{
						Entity entity = m_SceneHierarchy.GetContext()->CreateCircle("Circle");
						m_SceneHierarchy.SetSelectedEntity(entity);
					}
					ImGui::Separator();

					ImGui::Text("3D"); ImGui::Separator();
					if (ImGui::BeginMenu("Light"))
					{
						if (ImGui::MenuItem("Directional Light"))
						{
							Entity entity = m_SceneHierarchy.GetContext()->CreateDirectionalLight();
							m_SceneHierarchy.SetSelectedEntity(entity);
						}
						if (ImGui::MenuItem("Point Light"))
						{
							Entity entity = m_SceneHierarchy.GetContext()->CreatePointlight();
							m_SceneHierarchy.SetSelectedEntity(entity);
						}
						if (ImGui::MenuItem("Spot Light"))
						{
							Entity entity = m_SceneHierarchy.GetContext()->CreateSpotLight();
							m_SceneHierarchy.SetSelectedEntity(entity);
						}

						ImGui::EndMenu();
					}
					if (ImGui::BeginMenu("MESH"))
					{
						if (ImGui::MenuItem("Empty Mesh"))
						{
							Entity entity = m_SceneHierarchy.GetContext()->CreateMesh("Empty Mesh");
							m_SceneHierarchy.SetSelectedEntity(entity);
						}

						ImGui::EndMenu();
					}
				}

				// Entity Properties
				if (m_VpMenuContext == ViewportMenuContext::EntityProperties)
				{
					// Entity Properties
					std::string name = "None";
					m_SelectedEntity == m_SceneHierarchy.GetSelectedEntity() ?
						name = m_SelectedEntity.GetComponent<TagComponent>().Tag : name;

					ImGui::Text("%s", name.c_str());
					ImGui::Separator();

					if (ImGui::MenuItem("Delete"))
					{
						m_SelectedEntity = m_SceneHierarchy.GetSelectedEntity();
						m_SceneHierarchy.DestroyEntity(m_SelectedEntity);

						m_HoveredEntity = {};
					}

					if (ImGui::BeginMenu("Properties"))
					{
						ImGui::Text("Rename");
						if (m_SelectedEntity.HasComponent<TagComponent>())
						{
							auto& tag = m_SelectedEntity.GetComponent<TagComponent>().Tag;
							char buffer[64];
							strcpy_s(buffer, sizeof(buffer), tag.c_str());
							if (ImGui::InputText("##Tag", buffer, sizeof(buffer)))
							{
								tag = std::string(buffer);
								if (tag.empty()) tag = "'No Name'";
							}
						}

						if (m_SelectedEntity.HasComponent<SpriteRendererComponent>())
						{
							auto& component = m_SelectedEntity.GetComponent<SpriteRendererComponent>();
							ImGui::Separator();
							ImGui::ColorEdit4("Color", glm::value_ptr(component.Color));
						}

						if (m_SelectedEntity.HasComponent<SpriteRenderer2DComponent>())
						{
							auto& component = m_SelectedEntity.GetComponent<SpriteRenderer2DComponent>();
							ImGui::Separator();

							ImGui::ColorEdit4("Color", glm::value_ptr(component.Color));
							if (component.Texture)
							{
								ImGui::Text("Texture");
								ImGui::DragFloat2("Tilling Factor", glm::value_ptr(component.TillingFactor), 0.1f, 0.0f, 10.0f);
								if (ImGui::Button("Delete", ImVec2(64.0f, 24.0f)))
								{
									std::shared_ptr<Texture2D> texture = AssetManager::GetAsset<Texture2D>(component.Texture);
									texture->Delete();
									texture = {};
								}
							}
						}

						if (m_SelectedEntity.HasComponent<CircleRendererComponent>())
						{
							auto& component = m_SelectedEntity.GetComponent<CircleRendererComponent>();
							ImGui::Separator();
							ImGui::ColorEdit4("Color", glm::value_ptr(component.Color));
							ImGui::DragFloat("Thickness", &component.Thickness, 0.025f, 0.0f, 1.0f);
							ImGui::DragFloat("Fade", &component.Fade, 0.025f, 0.0f, 1.0f);
						}
						ImGui::EndMenu(); //!Properties
					}
				}
				ImGui::EndPopup();
			}
		}
		ImGui::PopStyleVar();
	}

	void EditorLayer::GUIRender()
	{
		if (guiAudioLibraryWindow)
		{
			ImGui::Begin("Audio Library", &guiAudioLibraryWindow);
			ImGui::Text("List: ");
			for (const auto& map : AudioEngine::GetMap())
				ImGui::Text("%s", map.first.c_str());

			if (ImGui::Button("Clear All"))
				AudioEngine::GetMap().clear();

			ImGui::End();
		}

		if (guiAudioCreationWindow)
		{
			ImGui::Begin("Audio Creation", &guiAudioCreationWindow);

			ImGui::End();
		}

		if (guiAnimationWindow)
		{
			ImGui::Begin("Animator", &guiAnimationWindow);
			if (Entity entity = m_SceneHierarchy.GetSelectedEntity())
			{
				if (entity.HasComponent<AnimationComponent>())
				{
					ImGui::Text("%s Animator", entity.GetTag().c_str());

					auto& ac = entity.GetComponent<AnimationComponent>();
					auto& state = ac.State;

					// Insert State name
					static std::string stateName;

					ImGui::Text("State Name: "); ImGui::SameLine();

					char buffer[256];
					strcpy_s(buffer, sizeof(buffer), stateName.c_str());

					if (ImGui::InputText("##stateName", buffer, sizeof(buffer)))
						stateName = std::string(buffer);

					ImGui::SameLine();
					if (ImGui::Button("+", ImVec2(30.0f, 20.0f)))
					{
						if (!stateName.empty())
						{
							state.AddState(stateName);
							stateName.clear();
						}
					}

					if (entity.HasComponent<SpriteRenderer2DComponent>() && state.HasState())
					{
						ImGui::Text("Animation State");

						ImGui::SameLine();

						// drop-down
						auto& stateStorage = state.GetStateStorage();
						std::string currentState = state.GetCurrentState();

						if (ImGui::BeginCombo("##AnimationState", currentState.c_str()))
						{
							bool isSelected = false;
							for (const auto& st : stateStorage)
							{
								isSelected = currentState == st;
								if (ImGui::Selectable(st.c_str(), isSelected))
								{
									currentState = st;
									state.SetActiveState(st);
								}
								if (isSelected)
									ImGui::SetItemDefaultFocus();
							}
							ImGui::EndCombo();
						} // !drop-down

						static Animation animation;

						// Drag and Drop
						ImGui::Button("Drop Texture", ImVec2(80.0f, 30.0f));
						if (ImGui::BeginDragDropTarget())
						{
							if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("CONTENT_BROWSER_ITEM"))
							{
								const auto* path = static_cast<const wchar_t*>(payload->Data);
								const std::filesystem::path texturePath = Project::GetActiveAssetFileSystemPath(path);

								if (texturePath.extension() == ".png" || texturePath.extension() == ".jpg")
								{
									const std::shared_ptr<Texture2D> texture = Texture2D::Create(texturePath.string());
									animation.AddFrame(texture, 0.23f);
								}
							}
						}

						ImGui::SameLine();
						if (ImGui::Button("Add Animation", { 90.0f, 30.0f }) && animation.HasFrame())
						{
							state.AddAnimation(animation);
							OGN_CORE_TRACE("Animation added to {}", stateName);

							animation.Delete();
						}

						ImGui::SameLine();
						if (ImGui::Button("Set As Default State", ImVec2(80.0f, 30.f)))
							state.SetDefaultState(state.GetCurrentState());

						// Display what texture has been dropped
						const float padding = 10.0f;
						const float imageSize = 42.0f;
						const float cellSize = imageSize + padding;

						const float panelWidth = ImGui::GetContentRegionAvail().x;
						int columnCount = static_cast<int>(panelWidth / cellSize);

						if (columnCount < 1)
							columnCount = 1;

						ImGui::Columns(columnCount, nullptr, false);

						if (animation.HasFrame())
						{
							for (int i = 0; i < animation.GetTotalFrames(); i++)
							{
								const ImTextureID animTexture = reinterpret_cast<ImTextureID>(animation.GetSprites(i)->GetRendererID());
								ImGui::Image(animTexture, ImVec2(imageSize, imageSize), ImVec2(0, 1), ImVec2(1, 0));

								ImGui::TextWrapped("Frame %d", i + 1);

								ImGui::NextColumn();
							}
							ImGui::Columns();
						}

						if (state.HasAnimation())
						{
							for (int i = 0; i < state.GetAnimation().GetTotalFrames(); i++)
							{
								const ImTextureID animTexture = reinterpret_cast<ImTextureID>(state.GetAnimation().GetSprites(i)->GetRendererID());
								ImGui::Image(animTexture, ImVec2(imageSize, imageSize), ImVec2(0, 1), ImVec2(1, 0));

								const int currentIndex = state.GetAnimation().GetFrameIndex();

								if (currentIndex == i)
									ImGui::TextWrapped("  -----");

								ImGui::NextColumn();
							}
							ImGui::Columns();

							bool looping = state.IsLooping();
							if (ImGui::Checkbox("Loop", &looping))
								state.SetLooping(looping);
							ImGui::SameLine();
							if (ImGui::Button("Preview"))
								state.Preview = !state.Preview;
							ImGui::SameLine();
							if (ImGui::Button("Delete State"))
								state.RemoveState(currentState);
						}
					}
				}

			}
			ImGui::End();
		}

		if (guiRenderStatusWindow)
		{
			ImGui::Begin("Render Status", &guiRenderStatusWindow);

			const char* CMSTypeString[] = { "PIVOT", "FREE MOVE" };
			const char* currentCMSTypeString = CMSTypeString[static_cast<int>(m_EditorCamera.GetStyle())];
			ImGui::Text("Distance %.5f", m_EditorCamera.GetDistance());

			if (ImGui::BeginCombo("CAMERA STYLE", currentCMSTypeString)) {
				for (int i = 0; i < 2; i++) {
					const bool isSelected = currentCMSTypeString == CMSTypeString[i];
					if (ImGui::Selectable(CMSTypeString[i], isSelected)) {
						currentCMSTypeString = CMSTypeString[i];
						m_EditorCamera.SetStyle(static_cast<CameraStyle>(i));

					} if (isSelected) ImGui::SetItemDefaultFocus();
				} ImGui::EndCombo();
			}
			if (ImGui::SliderFloat("FOV", &m_CameraFov, 0.0f, 90.0f))
				m_EditorCamera.SetFov(m_CameraFov);

			ImGui::Checkbox("Visualize Colliders", &m_VisualizeCollider);

			ImGui::Text("Grid");
			ImGui::Text("Size "); ImGui::SameLine(0.0f, 1.5f); ImGui::DragInt("##grid_size", &m_GridSize);
			ImGui::Text("Color"); ImGui::SameLine(0.0f, 1.5f); ImGui::ColorEdit4("##grid_color", glm::value_ptr(m_GridColor));
			m_ActiveScene->SetGrid(m_GridSize, m_GridColor);

			ImGui::Separator();

			const auto Stats2D = Renderer2D::GetStats();
			ImGui::Text("2D Stats");
			ImGui::Text("Draw Calls: %d", Stats2D.DrawCalls);
			ImGui::Text("Quads %d", Stats2D.QuadCount);
			ImGui::SameLine(); ImGui::Text("Circles %d", Stats2D.CircleCount);
			ImGui::SameLine(); ImGui::Text("Lines %d", Stats2D.LineCount);
			ImGui::Text("Vertices: %d", Stats2D.GetTotalVertexCount());
			ImGui::Text("Indices: %d", Stats2D.GetTotalIndexCount());
			ImGui::Separator();

			const auto Stats3D = Renderer3D::GetStats();
			ImGui::Text("3D Stats");
			ImGui::Text("Draw Calls: %d", Stats3D.DrawCalls);
			ImGui::Text("Cube %d", Stats3D.CubeCount);
			ImGui::Text("Vertices: %d", Stats3D.GetTotalVertexCount());
			ImGui::Text("Indices: %d", Stats3D.GetTotalIndexCount());
			ImGui::Separator();

			if (ImGui::Checkbox("Line Mode", &drawLineMode)) RenderCommand::DrawLineMode(drawLineMode);
			ImGui::SameLine(0.0f, 1.5f); ImGui::ColorEdit4("Background Color", glm::value_ptr(clearColor));

			const char* RTTypeString[] = { "Normal" };
			const char* currentRTTypeString = RTTypeString[m_RenderTarget];

			if (ImGui::BeginCombo("Render Target", currentRTTypeString)) {
				for (int i = 0; i < 1; i++) {
					const bool isSelected = currentRTTypeString == RTTypeString[i];
					if (ImGui::Selectable(RTTypeString[i], isSelected)) {
						currentRTTypeString = RTTypeString[i];
						m_RenderTarget = i;
					} if (isSelected) ImGui::SetItemDefaultFocus();
				} ImGui::EndCombo();
			}

			ImGui::End();
		}

		if (guiMenuStyle)
		{
			ImGui::Begin("Style Editor", &guiMenuStyle);
			ImGui::ShowStyleEditor();
			ImGui::End();
		}

		if (guiDebugInfo)
		{
			ImGui::Begin("Debug Info", &guiDebugInfo);

			ImGui::Text("Time (%.2f s)", m_Time);
			if (ImGui::Button("Reset Time")) { m_Time = 0.0f; }
			ImGui::Text("OpenGL Version : (%s)", glGetString(GL_VERSION));
			ImGui::Text("ImGui version : (%s)", IMGUI_VERSION);
			ImGui::Text("ImGuizmo Hovered (%d)", ImGuizmo::IsOver());
			ImGui::Text("Viewport Hovered (%d)", m_SceneViewportHovered);
			ImGui::Text("Hierarchy Menu Activity (%d)", m_SceneHierarchy.GetHierarchyMenuActive());
			ImGui::End();
		}

		if (guiImGuiDemoWindow)
		{
			ImGui::ShowDemoWindow(&guiImGuiDemoWindow);
		}
	}

	bool EditorLayer::OnMouseButtonPressed(MouseButtonPressedEvent& e)
	{
		bool control = Input::IsKeyPressed(Key::LeftControl);

		if (e.GetMouseButton() == Mouse::ButtonLeft && m_SceneViewportHovered)
		{
			if (!ImGuizmo::IsOver() && !control || ImGuizmo::IsOver() && m_GizmosType == -1 && !control)
			{
				if (m_HoveredEntity != m_SelectedEntity)
				{
					m_SceneHierarchy.SetSelectedEntity(m_HoveredEntity);
				}

				if (!m_HoveredEntity || m_HoveredEntity == m_SelectedEntity)
				{
					m_GizmosType = -1;
					m_SceneHierarchy.SetSelectedEntity({});
				}
			}

			// Changing Gizmo Type
			if (!ImGuizmo::IsOver() && m_HoveredEntity == m_SelectedEntity && control)
			{
				if (m_GizmosMode == ImGuizmo::MODE::LOCAL)
				{
					m_GizmosType == -1 ? m_GizmosType = ImGuizmo::OPERATION::TRANSLATE : m_GizmosType == ImGuizmo::OPERATION::TRANSLATE ?
						m_GizmosType = ImGuizmo::OPERATION::ROTATE : m_GizmosType == ImGuizmo::OPERATION::ROTATE ? m_GizmosType = ImGuizmo::OPERATION::SCALE : m_GizmosType = -1;
				}
				else if (m_GizmosMode == ImGuizmo::MODE::WORLD)
				{
					m_GizmosType == -1 ? m_GizmosType = ImGuizmo::OPERATION::TRANSLATE : m_GizmosType == ImGuizmo::OPERATION::TRANSLATE ?
						m_GizmosType = ImGuizmo::OPERATION::ROTATE : m_GizmosType == ImGuizmo::OPERATION::ROTATE ? m_GizmosType = -1 : m_GizmosType = ImGuizmo::OPERATION::TRANSLATE;
				}
			}
		}
		return false;
	}

	void EditorLayer::InputProcedure(Timestep time)
	{
		if (Input::IsMouseButtonPressed(Mouse::ButtonRight))
		{
			RMHoldTime += time.DeltaTime();

			if (lastMouseX == mouseX && lastMouseY == mouseY) VpMenuContextActive = true;
			else if (lastMouseX != mouseX && lastMouseY != mouseY) VpMenuContextActive = false;

			if (lastMouseX == mouseX && lastMouseY == mouseY)
			{
				if (m_HoveredEntity == m_SelectedEntity && m_HoveredEntity)
					m_VpMenuContext = ViewportMenuContext::EntityProperties;
				if (m_HoveredEntity != m_SelectedEntity || !m_HoveredEntity)
					m_VpMenuContext = ViewportMenuContext::CreateMenu;
			}
		}
		else
		{
			lastMouseX = mouseX;
			lastMouseY = mouseY;
			RMHoldTime = 0.0f;
		}
	}

	bool EditorLayer::OnKeyPressed(KeyPressedEvent& e)
	{
		auto& app = Application::Get();
		const bool control = Input::IsKeyPressed(Key::LeftControl) || Input::IsKeyPressed(Key::RightControl);
		const bool shift = Input::IsKeyPressed(Key::LeftShift) || Input::IsKeyPressed(Key::RightShift);

		ImGuiIO& io = ImGui::GetIO();

		switch (e.GetKeyCode())
		{
			// File Operation
			case Key::S:
			{
				if (control)
				{
					if (shift)
						SaveSceneAs();
					else
						SaveScene();
				}
				break;
			}

			case Key::O:
			{
				if (control) OpenScene();
				break;
			}

			case Key::N:
			{
				if (control) NewScene();
				break;
			}

			case Key::T:
			{
				if (!ImGuizmo::IsUsing() && !io.WantTextInput)
					m_GizmosType = ImGuizmo::OPERATION::TRANSLATE;
				break;
			}

			case Key::D:
			{
				if (control)
					OnDuplicateEntity();
				break;
			}

			case Key::E:
			{
				if (!ImGuizmo::IsUsing() && !io.WantTextInput && m_GizmosMode == ImGuizmo::MODE::LOCAL)
					m_GizmosType = ImGuizmo::OPERATION::SCALE;
				break;
			}

			case Key::R:
			{
				if (!ImGuizmo::IsUsing() && !io.WantTextInput)
					m_GizmosType = ImGuizmo::OPERATION::ROTATE;
				break;
			}

			case Key::F11:
			{
				guiMenuFullscreen == false ? guiMenuFullscreen = true : guiMenuFullscreen = false;
				app.GetWindow().SetFullscreen(guiMenuFullscreen);
				break;
			}

			case Key::Delete:
			{
				if (Application::Get().GetGuiLayer()->GetActiveWidgetID() == 0)
				{
					Entity selectedEntity = m_SceneHierarchy.GetSelectedEntity();
					if (selectedEntity)
					{
						m_SceneHierarchy.SetSelectedEntity({});
						m_SceneHierarchy.DestroyEntity(selectedEntity);
					}
				}
				break;
			}
		}

		return false;
	}

	bool EditorLayer::OnWindowDrop(WindowDropEvent& e)
	{
		// AssetManager::ImportAsset();
		return true;
	}

	bool EditorLayer::OnWindowResize(WindowResizeEvent& e)
	{
		return false;
	}

	bool EditorLayer::OnMouseMovedEvent(MouseMovedEvent& e)
	{
		return false;
	}

	bool EditorLayer::OnMouseButtonEvent(MouseButtonEvent& e)
	{
		return false;
	}
}
