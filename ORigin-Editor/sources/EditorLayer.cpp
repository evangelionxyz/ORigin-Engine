// Copyright (c) Evangelion Manuhutu | ORigin Engine
#include "EditorLayer.h"
#include "Gizmos/Gizmos.h"
#include "Origin/EntryPoint.h"
#include "Origin/Utils/PlatformUtils.h"
#include "Origin/Scene/SceneSerializer.h"
#include "Origin/Scripting/ScriptEngine.h"
#include "Origin/Renderer/Font.h"
#include "Origin/Asset/AssetManager.h"
#include "Origin/Audio/Audio.h"
#include "Origin/Utils/Utils.h"
#include "Origin/Asset/AssetImporter.h"
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <imgui_internal.h>
#include <filesystem>
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/quaternion.hpp>
#include <glm/gtx/compatibility.hpp>
#pragma warning(disable : OGN_DISABLED_WARNINGS)

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
	  // Load UI Textures
	  m_UITextures["play"] = TextureImporter::LoadTexture2D("Resources/UITextures/playbutton.png");
	  m_UITextures["simulate"] = TextureImporter::LoadTexture2D("Resources/UITextures/simulatebutton.png");
	  m_UITextures["stop"] = TextureImporter::LoadTexture2D("Resources/UITextures/stopbutton.png");
	  m_UITextures["pause"] = TextureImporter::LoadTexture2D("Resources/UITextures/pausebutton.png");
	  m_UITextures["stepping"] = TextureImporter::LoadTexture2D("Resources/UITextures/steppingframebutton.png");
	  m_UITextures["camera_2d_projection"] = TextureImporter::LoadTexture2D("Resources/UITextures/camera_projection_2d_icon.png");
	  m_UITextures["camera_3d_projection"] = TextureImporter::LoadTexture2D("Resources/UITextures/camera_projection_3d_icon.png");
	  m_UITextures["camera"] = TextureImporter::LoadTexture2D("Resources/UITextures/camera.png");
	  m_UITextures["lighting"] = TextureImporter::LoadTexture2D("Resources/UITextures/lighting.png");
	  m_UITextures["audio"] = TextureImporter::LoadTexture2D("Resources/UITextures/audio.png");

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

	  m_EditorCamera.InitPerspective(45.0f, 1.778f, 0.1f, 5000.0f);
	  m_EditorCamera.InitOrthographic(10.0f, -1.0f, 100.0f);
	  m_EditorCamera.SetPosition(glm::vec3(0.0f, 1.0f, 10.0f));

	  m_ActiveScene = std::make_shared<Scene>();
	  const auto& commandLineArgs = Application::Get().GetSpecification().CommandLineArgs;
	  if (commandLineArgs.Count > 1)
	  {
		  m_ProjectDirectoryPath = commandLineArgs[1];
		  if (!OpenProject(m_ProjectDirectoryPath))
			  Application::Get().Close();
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

  void EditorLayer::OnUpdate(Timestep ts)
  {
	  Renderer2D::ResetStats();
	  Renderer3D::ResetStats();

	  InputProcedure(ts);

	  m_Time += ts.Seconds();
	  const bool enableCamera = !ImGuizmo::IsUsing() && !ImGui::GetIO().WantTextInput && Application::Get().GetGuiLayer()->GetActiveWidgetID() == 0;
	  m_EditorCamera.EnableMovement(enableCamera);
		m_ActiveScene->OnShadowRender();

		switch (m_SceneState)
		{
		case origin::EditorLayer::SceneState::Edit:
		case origin::EditorLayer::SceneState::Simulate:
			if (const FramebufferSpecification spec = m_Framebuffer->GetSpecification();
				m_SceneViewportSize.x > 0.0f && m_SceneViewportSize.y > 0.0f && (m_SceneViewportSize.x != spec.Width || m_SceneViewportSize.y != spec.Height))
			{
				m_Framebuffer->Resize(static_cast<uint32_t>(m_SceneViewportSize.x), static_cast<uint32_t>(m_SceneViewportSize.y));
				m_EditorCamera.SetViewportSize(m_SceneViewportSize.x, m_SceneViewportSize.y);
				m_ActiveScene->OnViewportResize(static_cast<uint32_t>(m_SceneViewportSize.x), static_cast<uint32_t>(m_SceneViewportSize.y));
			}
			break;
		case origin::EditorLayer::SceneState::Play:
			m_ActiveScene->OnViewportResize(static_cast<uint32_t>(m_SceneViewportSize.x), static_cast<uint32_t>(m_SceneViewportSize.y));
			break;
		}

	  m_Framebuffer->Bind();
	  RenderCommand::Clear();
	  RenderCommand::ClearColor(clearColor);
	  m_Framebuffer->ClearAttachment(1, -1);
	  Draw(ts);
	  auto [mx, my] = ImGui::GetMousePos();
	  glm::vec2 mousePos = { mx, my };
	  mousePos -= m_SceneViewportBounds[0];
	  const glm::vec2& viewportSize = m_SceneViewportBounds[1] - m_SceneViewportBounds[0];
	  mousePos.y = viewportSize.y - mousePos.y;
	  mousePos = glm::clamp(mousePos, glm::vec2(0.0f), viewportSize - glm::vec2(1.0f));
	  mouseX = static_cast<int>(mousePos.x);
	  mouseY = static_cast<int>(mousePos.y);
	  m_PixelData = m_Framebuffer->ReadPixel(1, mouseX, mouseY);
	  m_HoveredEntity = m_PixelData == -1 ? Entity() : Entity(static_cast<entt::entity>(m_PixelData), m_ActiveScene.get());
	  m_Framebuffer->Unbind();
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
	  SceneViewport();
	  MenuBar();
	  GUIRender();
	  if (m_ContentBrowser)
		  m_ContentBrowser->OnImGuiRender();
	  m_SceneHierarchy.OnImGuiRender();
	  m_Dockspace.End();
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
	  ScriptEngine::ClearSceneContext();

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
	  if (Project::New())
	  {
		  ScriptEngine::Init();

		  m_ProjectDirectoryPath = Project::GetActiveProjectDirectory();

		  m_ContentBrowser = std::make_unique<ContentBrowserPanel>(Project::GetActive());
		  if (!m_ContentBrowser)
		  {
			  OGN_CORE_ERROR("Editor Layer: ContentBrowserPanel Failed to initialized");
			  return false;
		  }
		  NewScene();
	  }
	  return true;
  }

  bool EditorLayer::OpenProject(const std::filesystem::path& path)
  {
	  if (Project::Load(path))
	  {
		  ScriptEngine::Init();

		  AssetHandle handle = Project::GetActive()->GetConfig().StartScene;
		  OpenScene(handle);

		  m_ProjectDirectoryPath = Project::GetActiveProjectDirectory();

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
		  OpenScene(handle);

		  m_ProjectDirectoryPath = Project::GetActiveProjectDirectory();

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
	  Project::SaveActive();
  }

  void EditorLayer::NewScene()
  {
	  if (m_SceneState == SceneState::Play || m_SceneState == SceneState::Simulate)
		  OnSceneStop();

	  m_EditorScene = std::make_shared<Scene>();
	  m_SceneHierarchy.SetContext(m_EditorScene, true);

	  m_ActiveScene = m_EditorScene;
	  m_ScenePath = std::filesystem::path();
  }

  void EditorLayer::SaveScene()
  {
	  if (m_SceneState == SceneState::Play || m_SceneState == SceneState::Simulate)
		  OnSceneStop();

	  if (!m_ScenePath.empty())
	  {
		  SerializeScene(m_ActiveScene, m_ScenePath);
	  }
	  else
	  {
		  SaveSceneAs();
	  }

	  Project::GetActive()->GetEditorAssetManager()->SerializeAssetRegistry();
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
	  if (!AssetManager::IsAssetHandleValid(handle) || handle == 0)
	  {
		  EditorLayer::NewScene();
		  return;
	  }

	  if (m_SceneState != SceneState::Edit)
		  OnSceneStop();

	  std::shared_ptr<Scene> readOnlyScene = AssetManager::GetAsset<Scene>(handle);

	  if (!readOnlyScene)
	  {
		  OGN_CORE_ERROR("EditorLayer: Invalid Scene");
		  return;
	  }

	  m_EditorScene = Scene::Copy(readOnlyScene);
	  m_HoveredEntity = {};

	  m_SceneHierarchy.SetContext(m_EditorScene, true);
	  m_ActiveScene = m_EditorScene;
	  m_ScenePath = Project::GetActive()->GetEditorAssetManager()->GetFilepath(handle);

		m_ActiveScene->OnViewportResize(static_cast<uint32_t>(m_SceneViewportSize.x), static_cast<uint32_t>(m_SceneViewportSize.y));
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
	  if (!readOnlyScene)
	  {
		  OGN_CORE_ERROR("EditorLayer: Invalid Scene");
		  return;
	  }

	  m_HoveredEntity = {};
	  m_EditorScene = Scene::Copy(readOnlyScene);
	  m_SceneHierarchy.SetContext(m_EditorScene, true);
	  m_ActiveScene = m_EditorScene;
	  m_ScenePath = Project::GetActive()->GetEditorAssetManager()->GetFilepath(handle);
		m_ActiveScene->OnViewportResize(static_cast<uint32_t>(m_SceneViewportSize.x), static_cast<uint32_t>(m_SceneViewportSize.y));
  }

  void EditorLayer::SerializeScene(std::shared_ptr<Scene> scene, const std::filesystem::path filepath)
  {
    SceneImporter::SaveScene(scene, filepath);
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
				if (ImGui::MenuItem("Save Project", "", nullptr, (bool)Project::GetActive())) SaveProject();
				ImGui::Separator();
				if (ImGui::MenuItem("New Scene", "Ctrl+N", nullptr, (bool)Project::GetActive())) NewScene();
				if (ImGui::MenuItem("Open Scene", "Ctrl+O", nullptr, (bool)Project::GetActive()))  OpenScene();
				if (ImGui::MenuItem("Save Scene", "Ctrl+S", nullptr, (bool)Project::GetActive()))  SaveScene();
				if (ImGui::MenuItem("Save Scene As", "Ctrl+Shift+S", nullptr, (bool)Project::GetActive()))  SaveSceneAs();
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
				if (ImGui::MenuItem("Full Screen", "F11", &guiMenuFullscreen))
				{
					window.GetWindow().SetFullscreen(guiMenuFullscreen);
				}
				ImGui::EndMenu();
			}

			ImGui::EndMenuBar();
			ImGui::PopStyleColor();
			ImGui::PopStyleVar();
		}
	}

	void EditorLayer::Draw(float deltaTime)
	{
		switch (m_SceneState)
		{
		case SceneState::Play:
			m_GizmosType = -1;
			m_ActiveScene->OnUpdateRuntime(deltaTime);
			break;

		case SceneState::Edit:
			m_EditorCamera.OnUpdate(deltaTime);
			m_ActiveScene->OnUpdateEditor(deltaTime, m_EditorCamera);
			Gizmos::OnUpdate(m_EditorCamera);
			break;

		case SceneState::Simulate:
			m_EditorCamera.OnUpdate(deltaTime);
			m_ActiveScene->OnUpdateSimulation(deltaTime, m_EditorCamera);
			Gizmos::OnUpdate(m_EditorCamera);
			break;
		}
	}

	void EditorLayer::SceneViewport()
	{
		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));
		ImGuiWindowFlags window_flags = ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse | ImGuiWindowFlags_NoCollapse;

		ImGui::Begin("Scene", nullptr, window_flags);

		SceneViewportMenu();
		SceneViewportToolbar();
		SceneViewportOverlay();

		m_SceneViewportHovered = ImGui::IsWindowHovered();
		m_SceneViewportFocused = ImGui::IsWindowFocused();

		Application::Get().GetGuiLayer()->BlockEvents(!m_SceneViewportHovered);
		m_SceneHierarchy.SetHierarchyMenuActive(!ImGui::IsWindowFocused());

		const ImVec2& viewportMinRegion = ImGui::GetWindowContentRegionMin();
		const ImVec2& viewportMaxRegion = ImGui::GetWindowContentRegionMax();
		const ImVec2& viewportOffset = ImGui::GetWindowPos();
		m_SceneViewportBounds[0] = { viewportMinRegion.x + viewportOffset.x, viewportMinRegion.y + viewportOffset.y };
		m_SceneViewportBounds[1] = { viewportMaxRegion.x + viewportOffset.x, viewportMaxRegion.y + viewportOffset.y };

		ImVec2& viewportPanelSize = ImGui::GetContentRegionAvail();
		m_SceneViewportSize = { viewportPanelSize.x, viewportPanelSize.y };
		
		if (m_SceneState == SceneState::Play)
		{
			auto& camView = m_ActiveScene->m_Registry.view<CameraComponent>();
			for (auto& e : camView)
			{
				auto cc = camView.get<CameraComponent>(e);

				if (cc.Primary)
				{
					switch (cc.Camera.GetAspectRatioType())
					{
					case SceneCamera::AspectRatioType::TwentyOneByNine:
						// keep the width and adjust the height to maintain 16:9 aspect ratio
						m_GameViewportSizeX = m_SceneViewportSize.x;
						m_GameViewportSizeY = m_GameViewportSizeX / 21.0f * 9.0f;

						// if the calculated height is greater than the available height, adjust the width
						if (m_GameViewportSizeY > m_SceneViewportSize.y)
						{
							m_GameViewportSizeY = m_SceneViewportSize.y;
							m_GameViewportSizeX = m_GameViewportSizeY / 9.0f * 21.0f;
						}
						break;
					case SceneCamera::AspectRatioType::SixteenByNine:
						// keep the width and adjust the height to maintain 16:9 aspect ratio
						m_GameViewportSizeX = m_SceneViewportSize.x;
						m_GameViewportSizeY = m_GameViewportSizeX / 16.0f * 9.0f;

						// if the calculated height is greater than the available height, adjust the width
						if (m_GameViewportSizeY > m_SceneViewportSize.y)
						{
							m_GameViewportSizeY = m_SceneViewportSize.y;
							m_GameViewportSizeX = m_GameViewportSizeY / 9.0f * 16.0f;
						}
						break;
					case SceneCamera::AspectRatioType::SixteenByTen:
						// keep the width and adjust the height to maintain 16:9 aspect ratio
						m_GameViewportSizeX = m_SceneViewportSize.x;
						m_GameViewportSizeY = m_GameViewportSizeX / 16.0f * 10.0f;

						// if the calculated height is greater than the available height, adjust the width
						if (m_GameViewportSizeY > m_SceneViewportSize.y)
						{
							m_GameViewportSizeY = m_SceneViewportSize.y;
							m_GameViewportSizeX = m_GameViewportSizeY / 10.0f * 16.0f;
						}
						break;
					case SceneCamera::AspectRatioType::FourByThree:
						// keep the width and adjust the height to maintain 16:9 aspect ratio
						m_GameViewportSizeX = m_SceneViewportSize.x;
						m_GameViewportSizeY = m_GameViewportSizeX / 4.0f * 3.0f;

						// if the calculated height is greater than the available height, adjust the width
						if (m_GameViewportSizeY > m_SceneViewportSize.y)
						{
							m_GameViewportSizeY = m_SceneViewportSize.y;
							m_GameViewportSizeX = m_GameViewportSizeY / 3.0f * 4.0f;
						}
						break;
					case SceneCamera::AspectRatioType::Free:
						m_GameViewportSizeX = m_SceneViewportSize.x;
						m_GameViewportSizeY = m_SceneViewportSize.y;
						break;
					}
				}
			}

			// centered image position
			float offsetX = (m_SceneViewportSize.x - m_GameViewportSizeX) * 0.5f;
			float offsetY = (m_SceneViewportSize.y - m_GameViewportSizeY) * 0.5f;
			ImGui::SetCursorPos({ offsetX, offsetY + 20.0f });
		}
		else
		{
			m_GameViewportSizeX = m_SceneViewportSize.x;
			m_GameViewportSizeY = m_SceneViewportSize.y;
		}

		ImTextureID viewportID = reinterpret_cast<ImTextureID>(m_Framebuffer->GetColorAttachmentRendererID(m_RenderTarget));
		ImGui::Image(viewportID, ImVec2(m_GameViewportSizeX, m_GameViewportSizeY), ImVec2(0, 1), ImVec2(1, 0));
		if (ImGui::BeginDragDropTarget())
		{
			if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("CONTENT_BROWSER_ITEM"))
			{
				if (m_SceneState == SceneState::Edit)
				{
					AssetHandle handle = *(AssetHandle*)payload->Data;
					OpenScene(handle);
				}
			}
			ImGui::EndDragDropTarget();
		}

		Entity entity = m_SceneHierarchy.GetSelectedEntity();
		if (entity && m_GizmosType != -1)
		{
			// Gizmos
			ImGuizmo::SetDrawlist();
			ImGuizmo::SetRect(m_SceneViewportBounds[0].x, m_SceneViewportBounds[0].y, m_SceneViewportBounds[1].x - m_SceneViewportBounds[0].x, m_SceneViewportBounds[1].y - m_SceneViewportBounds[0].y);
			ImGuizmo::SetOrthographic(m_EditorCamera.GetProjectionType() == ProjectionType::Orthographic);

			auto& tc = entity.GetComponent<TransformComponent>();
			glm::mat4 transform = tc.GetTransform();
			
			const glm::mat4& cameraProjection = m_EditorCamera.GetProjection();
			const glm::mat4& cameraView = m_EditorCamera.GetViewMatrix();

			float snapValue = 0.5f;
			if (m_GizmosType == ImGuizmo::OPERATION::ROTATE)
				snapValue = 15.0f;
			else if (m_GizmosType == ImGuizmo::OPERATION::BOUNDS)
				snapValue = 0.1f;
				
			float snapValues[] = { snapValue, snapValue, snapValue };
			float bounds[] = { -0.5f, -0.5f, -0.5f, 0.5f, 0.5f, 0.5f };
			bool boundSizing = m_GizmosType == ImGuizmo::OPERATION::BOUNDS
				&& !entity.HasComponent<CameraComponent>()
				&& !entity.HasComponent<LightComponent>()
				&& !entity.HasComponent<AudioComponent>();

			bool snap = Input::Get().IsKeyPressed(KeyCode(Key::LeftShift));
			ImGuizmo::Manipulate(glm::value_ptr(cameraView), glm::value_ptr(cameraProjection),
				static_cast<ImGuizmo::OPERATION>(m_GizmosType), static_cast<ImGuizmo::MODE>(m_GizmosMode),
				glm::value_ptr(transform), nullptr, snap ? snapValues : nullptr, boundSizing ? bounds : nullptr, snap ? snapValues : nullptr);

			if (ImGuizmo::IsUsing())
			{
				glm::vec3 translation, rotation, scale;
				Math::DecomposeTransformEuler(transform, translation, rotation, scale);
				tc.Translation = translation;
				glm::vec3 deltaRotation = rotation - tc.Rotation;
				tc.Rotation += deltaRotation;
				tc.Scale = scale;
			}

			if (ImGui::IsWindowFocused() && Input::Get().IsKeyPressed(Key::Escape))
				m_GizmosType = -1;
		}

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
				std::shared_ptr<Texture2D> icon = (m_SceneState == SceneState::Edit || m_SceneState == SceneState::Simulate) ? m_UITextures.at("play") : m_UITextures.at("stop");
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
				std::shared_ptr<Texture2D> icon = (m_SceneState == SceneState::Edit || m_SceneState == SceneState::Play) ? m_UITextures.at("simulate") : m_UITextures.at("stop");
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
				std::shared_ptr<Texture2D> icon = m_UITextures.at("pause");
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
					std::shared_ptr<Texture2D> icon = m_UITextures.at("stepping");
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

		ImGui::SetNextWindowPos({ (viewportMinRegion.x + viewportOffset.x) + wndWidth - 145.0f, wndYpos }, ImGuiCond_Always);
		ImGui::SetNextWindowBgAlpha(0.0f);
		if (ImGui::Begin("##tool_bar", nullptr, window_flags))
		{
			// Camera Projection Mode Button
			ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.0f, 0.0f, 0.0f, 0.0f));
			ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(1.0f, 1.0f, 1.0f, 1.0f));

			auto mode = m_EditorCamera.GetProjectionType();
			std::shared_ptr<Texture2D> cameraPrjModeButton = mode == ProjectionType::Orthographic ? m_UITextures.at("camera_2d_projection") : m_UITextures.at("camera_3d_projection");
			if (ImGui::ImageButton(reinterpret_cast<ImTextureID>(cameraPrjModeButton->GetRendererID()), ImVec2(25.0f, 25.0f), ImVec2(0, 1), ImVec2(1, 0)))
			{
				if(mode == ProjectionType::Perspective)
					m_EditorCamera.SetProjectionType(ProjectionType::Orthographic);
				else if(mode == ProjectionType::Orthographic)
					m_EditorCamera.SetProjectionType(ProjectionType::Perspective);
			}
			ImGui::SameLine(0.0f, 5.0f);
			
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
			ImGui::SameLine(0.0f, 5.0f);

			ImGui::PushStyleColor(ImGuiCol_Button, btGLOBAL);
			ImGui::PushStyleColor(ImGuiCol_ButtonHovered, btGLOBAL);
			ImGui::PushStyleColor(ImGuiCol_ButtonActive, btActive);
			if (ImGui::Button("GLOBAL", { 50.0f, 25.0f }))
				m_GizmosMode = ImGuizmo::MODE::WORLD;
			ImGui::PopStyleColor(8);

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

					if (ImGui::MenuItem("Sprite"))
					{
						Entity entity = m_SceneHierarchy.GetContext()->CreateEntity("Sprite");
						entity.AddComponent<SpriteRenderer2DComponent>();
						m_SceneHierarchy.SetSelectedEntity(entity);
					}
					ImGui::Separator();

					if (ImGui::MenuItem("Lighting"))
					{
						Entity entity = m_SceneHierarchy.GetContext()->CreateEntity("Lighting");
						entity.AddComponent<LightComponent>().Light = Lighting::Create(LightingType::Directional);
						m_SceneHierarchy.SetSelectedEntity(entity);
					}

					if (ImGui::MenuItem("Empty Mesh"))
					{
						Entity entity = m_SceneHierarchy.GetContext()->CreateMesh("Empty Mesh");
						m_SceneHierarchy.SetSelectedEntity(entity);
					}
				}

				// Entity Properties
				if (m_VpMenuContext == ViewportMenuContext::EntityProperties)
				{
					// Entity Properties
					Entity selectedEntity = m_SceneHierarchy.GetSelectedEntity();
					if (selectedEntity)
					{
						auto& tag = selectedEntity.GetComponent<TagComponent>().Tag;
						ImGui::Text("%s", tag.c_str());
						ImGui::Separator();

						if (ImGui::MenuItem("Delete"))
						{
							m_SceneHierarchy.SetSelectedEntity({});
							m_SceneHierarchy.DestroyEntity(selectedEntity);
						}
					}
					
				}
				ImGui::EndPopup();
			}
		}
		ImGui::PopStyleVar();
	}

	void EditorLayer::SceneViewportOverlay()
	{
		const ImVec2& viewportMinRegion = ImGui::GetWindowContentRegionMin();
		const ImVec2& viewportOffset = ImGui::GetWindowPos();

		ImGuiWindowFlags window_flags = ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_AlwaysAutoResize
			| ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoFocusOnAppearing | ImGuiWindowFlags_NoNav;

		ImGui::SetNextWindowPos({ (viewportMinRegion.x + viewportOffset.x) + 8.0f, (viewportMinRegion.y + viewportOffset.y) + 8.0f }, ImGuiCond_Always);

		ImGui::SetNextWindowBgAlpha(0.0f); // Transparent background
		if (ImGui::Begin("##top_left_overlay", nullptr, window_flags))
		{
			if (Project::GetActive())
			{
				if (!m_SceneHierarchy.GetContext())
					ImGui::Text("Load a Scene or Create New Scene to begin!");
			}
			else
			{
				ImGui::Text("Create or Open a Project");
			}

			ImGui::Text("%.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
			ImGui::Text("Mouse Pos (%d, %d)", mouseX, mouseY);
		}

		ImGui::End();
	}

	void EditorLayer::GUIRender()
	{
		if (Entity entity = m_SceneHierarchy.GetSelectedEntity())
		{
			if (entity.HasComponent<AnimationComponent>())
			{
				ImGui::Begin("Animator", &guiAnimationWindow);
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

				if (entity.HasComponent<SpriteRenderer2DComponent>() && state.HasStates())
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
							AssetHandle handle = *(AssetHandle*)payload->Data;
							animation.AddFrame(handle, 0.23f);
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
							std::shared_ptr<Texture2D> texture = AssetManager::GetAsset<Texture2D>(animation.GetCurrentValue());
							const ImTextureID animTexture = reinterpret_cast<ImTextureID>(texture->GetRendererID());
							ImGui::Image(animTexture, ImVec2(imageSize, imageSize), ImVec2(0, 1), ImVec2(1, 0));

							ImGui::TextWrapped("Frame %d", i + 1);

							ImGui::NextColumn();
						}
						ImGui::Columns();
					}


					// Show the STATE animation
					if (state.HasAnimations())
					{
						for (int i = 0; i < state.GetAnimation().GetTotalFrames(); i++)
						{
							std::shared_ptr<Texture2D> texture = AssetManager::GetAsset<Texture2D>(state.GetAnimation().GetValue(i));
							const ImTextureID animTexture = reinterpret_cast<ImTextureID>(texture->GetRendererID());
							ImGui::Image(animTexture, ImVec2(imageSize, imageSize), ImVec2(0, 1), ImVec2(1, 0));

							const int currentIndex = state.GetAnimation().GetFrameIndex();

							if (currentIndex == i)
								ImGui::TextWrapped("  -----");

							ImGui::NextColumn();
						}
						ImGui::Columns();

						bool looping = state.IsLooping(currentState);
						if (ImGui::Checkbox("Loop", &looping))
							state.SetLooping(currentState, looping);

						ImGui::SameLine();
						if (ImGui::Button("Preview"))
							state.Preview = !state.Preview;

						ImGui::SameLine();
						if (ImGui::Button("Delete State"))
							state.RemoveState(currentState);
					}
				}
				ImGui::End();
			}
		}
		
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
		if (ImGui::DragFloat("FOV", &m_CameraFov, 1.0f, 0.0f, 90.0f))
			m_EditorCamera.SetFov(m_CameraFov);

		ImGui::Checkbox("Visualize Colliders", &m_VisualizeCollider);

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

		ImGui::SameLine(0.0f, 1.5f); ImGui::ColorEdit4("Background Color", glm::value_ptr(clearColor));

		const char* RTTypeString[] = { "Normal", "HDR" };
		const char* currentRTTypeString = RTTypeString[m_RenderTarget];

		if (ImGui::BeginCombo("Render Target", currentRTTypeString)) {
			for (int i = 0; i < 2; i++) {
				const bool isSelected = currentRTTypeString == RTTypeString[i];
				if (ImGui::Selectable(RTTypeString[i], isSelected)) {
					currentRTTypeString = RTTypeString[i];
					m_RenderTarget = i;
				} if (isSelected) ImGui::SetItemDefaultFocus();
			} ImGui::EndCombo();
		}

		ImGui::End();

		ImGui::Begin("Style Editor", &guiMenuStyle);
		ImGui::ShowStyleEditor();
		ImGui::End();

		ImGui::Begin("Debug Info", &guiDebugInfo);
		ImGui::Text("Time (%.2f s)", m_Time);
		if (ImGui::Button("Reset Time")) { m_Time = 0.0f; }
		ImGui::Text("OpenGL Version : (%s)", glGetString(GL_VERSION));
		ImGui::Text("ImGui version : (%s)", IMGUI_VERSION);
		ImGui::Text("ImGuizmo Hovered (%d)", ImGuizmo::IsOver());
		ImGui::Text("Viewport Hovered (%d)", m_SceneViewportHovered);
		ImGui::Text("Hierarchy Menu Activity (%d)", m_SceneHierarchy.GetHierarchyMenuActive());
		ImGui::End();

		ImGui::ShowDemoWindow(&guiImGuiDemoWindow);
	}

	bool EditorLayer::OnMouseButtonPressed(MouseButtonPressedEvent& e)
	{
		bool control = Input::Get().IsKeyPressed(Key::LeftControl);

		Entity selectedEntity = m_SceneHierarchy.GetSelectedEntity();

		if (e.GetMouseButton() == Mouse::ButtonLeft && m_SceneViewportHovered)
		{
			if (!ImGuizmo::IsOver() && !control)
			{
				uint32_t handle = (uint32_t)(entt::entity)m_HoveredEntity;
				bool handleValid = handle <= m_ActiveScene->m_EntityMap.size();

				if (handleValid)
				{
					if (m_HoveredEntity != selectedEntity)
						m_SceneHierarchy.SetSelectedEntity(m_HoveredEntity);
				}
				else if (!handleValid)
				{
					m_SceneHierarchy.SetSelectedEntity({});
					m_GizmosType = -1;
				}
			}

			if (!ImGuizmo::IsOver() && m_HoveredEntity == selectedEntity && control)
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
		auto delta = Input::Get().GetDeltaMouse();

		Entity entity = m_SceneHierarchy.GetSelectedEntity();

		if (Input::Get().IsMouseButtonPressed(Mouse::ButtonLeft))
		{
			float orthoSize = m_EditorCamera.GetOrthoSize();
			float orthoScale = orthoSize / m_SceneViewportSize.y;

			if (entity && m_SceneViewportHovered)
			{
				if (m_EditorCamera.GetProjectionType() == ProjectionType::Orthographic && !ImGuizmo::IsUsing())
				{
					auto& tc = entity.GetComponent<TransformComponent>();

					static glm::vec2 translate = tc.Translation;
					glm::vec2 velocity = glm::vec2(0.0f);

					if (entity.HasComponent<Rigidbody2DComponent>() && m_SceneState != SceneState::Edit)
					{
						auto rb2d = entity.GetComponent<Rigidbody2DComponent>();
						auto body = static_cast<b2Body*>(rb2d.RuntimeBody);
						velocity.x += delta.x * orthoScale * 5.0f;
						velocity.y -= delta.y * orthoScale * 5.0f;

						body->SetLinearVelocity(b2Vec2(velocity.x, velocity.y));
					}
					else
					{
						float snapSize = 0.5f;
						if (Input::Get().IsKeyPressed(Key::LeftShift))
						{
							translate.x += delta.x * orthoScale;
							translate.y -= delta.y * orthoScale;

							if (Input::Get().IsKeyPressed(Key::LeftControl))
								snapSize = 0.1f;

							tc.Translation.x = round(translate.x / snapSize) * snapSize;
							tc.Translation.y = round(translate.y / snapSize) * snapSize;
						}
						else
						{
							translate = glm::vec2(tc.Translation);

							translate.x += delta.x * orthoScale;
							translate.y -= delta.y * orthoScale;

							tc.Translation = glm::vec3(translate, tc.Translation.z);
						}
					}
				}
			}
		}

		if (Input::Get().IsMouseButtonPressed(Mouse::ButtonRight))
		{
			if (lastMouseX == mouseX && lastMouseY == mouseY) VpMenuContextActive = true;
			else if (lastMouseX != mouseX && lastMouseY != mouseY) VpMenuContextActive = false;

			if (lastMouseX == mouseX && lastMouseY == mouseY)
			{
				if (m_HoveredEntity == entity && m_HoveredEntity)
					m_VpMenuContext = ViewportMenuContext::EntityProperties;
				if (m_HoveredEntity != entity || !m_HoveredEntity)
					m_VpMenuContext = ViewportMenuContext::CreateMenu;
			}
		}
		else
		{
			lastMouseX = mouseX;
			lastMouseY = mouseY;
		}
	}

	bool EditorLayer::OnKeyPressed(KeyPressedEvent& e)
	{
		auto& app = Application::Get();
		const bool control = Input::Get().IsKeyPressed(Key::LeftControl) || Input::Get().IsKeyPressed(Key::RightControl);
		const bool shift = Input::Get().IsKeyPressed(Key::LeftShift) || Input::Get().IsKeyPressed(Key::RightShift);

		ImGuiIO& io = ImGui::GetIO();
		Entity selectedEntity = m_SceneHierarchy.GetSelectedEntity();

		switch (e.GetKeyCode())
		{
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

			case Key::F:
			{
				if (selectedEntity && !io.WantTextInput)
				{
					const auto& tc = selectedEntity.GetComponent<TransformComponent>();
					m_EditorCamera.SetPosition(tc.Translation);
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

			case Key::R:
			{
				if (!ImGuizmo::IsUsing() && !io.WantTextInput)
					m_GizmosType = ImGuizmo::OPERATION::ROTATE;
				break;
			}

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

			case Key::T:
			{
				if (!ImGuizmo::IsUsing() && !io.WantTextInput)
					m_GizmosType = ImGuizmo::OPERATION::TRANSLATE;
				break;
			}

			case Key::Y:
			{
				if (!ImGuizmo::IsUsing() && !io.WantTextInput)
					m_GizmosType = ImGuizmo::OPERATION::BOUNDS;
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
