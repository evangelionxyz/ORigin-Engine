// Copyright (c) Evangelion Manuhutu | ORigin Engine

#include "EditorLayer.h"
#include "Gizmos/Gizmos.h"
#include "Origin/EntryPoint.h"
#include "Origin/Utils/PlatformUtils.h"
#include "Origin/Scene/SceneSerializer.h"
#include "Origin/Scripting/ScriptEngine.h"
#include "Origin/Asset/AssetManager.h"
#include "Origin/Renderer/Font.h"
#include "Origin/Utils/Utils.h"
#include "Origin/Asset/AssetImporter.h"
#include "Origin/Scene/EntityManager.h"
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <imgui_internal.h>
#include <filesystem>
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/quaternion.hpp>
#include <glm/gtx/compatibility.hpp>

#include "Panels/AnimationTimeline.h"

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
	  m_UITextures["play"] = TextureImporter::LoadTexture2D("Resources/UITextures/play_icon.png");
	  m_UITextures["simulate"] = TextureImporter::LoadTexture2D("Resources/UITextures/simulate_icon.png");
	  m_UITextures["stop"] = TextureImporter::LoadTexture2D("Resources/UITextures/stop_icon.png");
	  m_UITextures["pause"] = TextureImporter::LoadTexture2D("Resources/UITextures/pause_icon.png");
	  m_UITextures["stepping"] = TextureImporter::LoadTexture2D("Resources/UITextures/stepping_icon.png");
	  m_UITextures["camera_2d_projection"] = TextureImporter::LoadTexture2D("Resources/UITextures/camera_projection_2d_icon.png");
	  m_UITextures["camera_3d_projection"] = TextureImporter::LoadTexture2D("Resources/UITextures/camera_projection_3d_icon.png");
	  m_UITextures["camera"] = TextureImporter::LoadTexture2D("Resources/UITextures/camera.png");
	  m_UITextures["lighting"] = TextureImporter::LoadTexture2D("Resources/UITextures/lighting.png");
	  m_UITextures["audio"] = TextureImporter::LoadTexture2D("Resources/UITextures/audio.png");
		m_OriginEngineTex = TextureImporter::LoadTexture2D("Resources/UITextures/origin_engine.png");

	  // ==============================
	  // Main Framebuffer Specification
	  // ==============================
	  FramebufferSpecification fbSpec;
	  fbSpec.Attachments =
	  {
		  FramebufferTextureFormat::RGBA8,
		  FramebufferTextureFormat::RED_INTEGER,
		  FramebufferTextureFormat::DEPTH24STENCIL8
	  };

	  fbSpec.Width = 1280;
	  fbSpec.Height = 720;
	  m_Framebuffer = Framebuffer::Create(fbSpec);

	  m_EditorCamera.InitOrthographic(10.0f, 0.1f, 100.0f);
	  m_EditorCamera.SetPosition(glm::vec3(0.0f, 1.0f, 10.0f));

	  m_ActiveScene = std::make_shared<Scene>();
	  const auto& commandLineArgs = Application::Get().GetSpecification().CommandLineArgs;
	  if (commandLineArgs.Count > 1)
	  {
		  m_ProjectDirectoryPath = commandLineArgs[1];
		  if (!OpenProject(m_ProjectDirectoryPath))
			  Application::Get().Close();
	  }

		m_Gizmos = std::make_unique<Gizmos>();
		m_SpriteSheetEditor = std::make_unique<SpriteSheetEditor>();
  }

  void EditorLayer::OnEvent(Event& e)
  {
		m_EditorCamera.OnEvent(e);
		m_SpriteSheetEditor->OnEvent(e);
		m_Gizmos->OnEvent(e);

    EventDispatcher dispatcher(e);
    dispatcher.Dispatch<MouseMovedEvent>(OGN_BIND_EVENT_FN(EditorLayer::OnMouseMovedEvent));
    dispatcher.Dispatch<WindowResizeEvent>(OGN_BIND_EVENT_FN(EditorLayer::OnWindowResize));
    dispatcher.Dispatch<KeyPressedEvent>(OGN_BIND_EVENT_FN(EditorLayer::OnKeyPressed));
    dispatcher.Dispatch<MouseButtonPressedEvent>(OGN_BIND_EVENT_FN(EditorLayer::OnMouseButtonPressed));
  }

	void EditorLayer::OnUpdate(Timestep ts)
  {
	  Renderer::GetStatistics().Reset();
		Application::Get().GetGuiLayer()->BlockEvents(!m_SceneViewportFocused && !m_SpriteSheetEditor->IsFocused);

	  InputProcedure(ts);

	  m_Time += ts.Seconds();

		// SpriteSheet Editor
		m_SpriteSheetEditor->OnRender(ts);

		m_ActiveScene->OnShadowRender();

		m_EditorCamera.SetMoveActive(!ImGui::GetIO().WantTextInput);
		m_EditorCamera.SetDraggingActive(m_SceneViewportFocused && !m_SpriteSheetEditor->IsFocused);
		m_EditorCamera.SetScrollingActive(m_SceneViewportHovered);

		switch (m_SceneState)
		{
		case EditorLayer::SceneState::Edit:
		case EditorLayer::SceneState::Simulate:
			if (const FramebufferSpecification spec = m_Framebuffer->GetSpecification();
				m_SceneViewportSize.x > 0.0f && m_SceneViewportSize.y > 0.0f && (m_SceneViewportSize.x != spec.Width || m_SceneViewportSize.y != spec.Height))
			{
				m_Framebuffer->Resize(static_cast<uint32_t>(m_SceneViewportSize.x), static_cast<uint32_t>(m_SceneViewportSize.y));
				m_EditorCamera.SetViewportSize(m_SceneViewportSize.x, m_SceneViewportSize.y);
				m_ActiveScene->OnViewportResize(static_cast<uint32_t>(m_SceneViewportSize.x), static_cast<uint32_t>(m_SceneViewportSize.y));
			}
			break;
		case EditorLayer::SceneState::Play:
			m_ActiveScene->OnViewportResize(static_cast<uint32_t>(m_SceneViewportSize.x), static_cast<uint32_t>(m_SceneViewportSize.y));
			break;
		}

	  RenderCommand::ClearColor(clearColor);
	  m_Framebuffer->Bind();
	  RenderCommand::Clear();
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
		m_Gizmos->SetHovered(m_PixelData);

	  m_Framebuffer->Unbind();
  }

  void EditorLayer::OnDuplicateEntity()
  {
	  if (m_SceneState != SceneState::Edit)
		  return;

	  if (Entity selectedEntity = m_SceneHierarchy.GetSelectedEntity())
	  {
			Entity entity = EntityManager::DuplicateEntity(selectedEntity, m_EditorScene.get());
		  m_SceneHierarchy.SetSelectedEntity(entity);
	  }
  }

  void EditorLayer::OnGuiRender()
  {
	  m_Dockspace.Begin();

	  SceneViewport();
	  MenuBar();
		SceneViewportToolbar();
	  GUIRender();

	  if (m_ContentBrowser)
		  m_ContentBrowser->OnImGuiRender();
	  m_SceneHierarchy.OnImGuiRender();

		m_SpriteSheetEditor->OnImGuiRender();

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
				ImGui::MenuItem("Render Settings", nullptr, &guiRenderSettingsWindow);
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
			m_Gizmos->SetType(GizmoType::NONE);
			m_ActiveScene->OnUpdateRuntime(deltaTime);
			break;

		case SceneState::Edit:
			m_Gizmos->OnRender(m_EditorCamera);
			m_EditorCamera.OnUpdate(deltaTime);
			m_ActiveScene->OnUpdateEditor(deltaTime, m_EditorCamera);
			break;

		case SceneState::Simulate:
			m_Gizmos->OnRender(m_EditorCamera);
			m_EditorCamera.OnUpdate(deltaTime);
			m_ActiveScene->OnUpdateSimulation(deltaTime, m_EditorCamera);
			break;
		}
	}

	void EditorLayer::SceneViewport()
	{
		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));
		ImGuiWindowFlags window_flags = ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse | ImGuiWindowFlags_NoCollapse;

		ImGui::Begin("Scene", nullptr, window_flags);

		SceneViewportMenu();
		
		m_SceneViewportHovered = ImGui::IsWindowHovered();
		m_SceneViewportFocused = ImGui::IsWindowFocused();

		
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
						m_GameViewportSizeX = m_SceneViewportSize.x;
						m_GameViewportSizeY = m_GameViewportSizeX / 21.0f * 9.0f;

						if (m_GameViewportSizeY > m_SceneViewportSize.y)
						{
							m_GameViewportSizeY = m_SceneViewportSize.y;
							m_GameViewportSizeX = m_GameViewportSizeY / 9.0f * 21.0f;
						}
						break;
					case SceneCamera::AspectRatioType::SixteenByNine:
						m_GameViewportSizeX = m_SceneViewportSize.x;
						m_GameViewportSizeY = m_GameViewportSizeX / 16.0f * 9.0f;

						if (m_GameViewportSizeY > m_SceneViewportSize.y)
						{
							m_GameViewportSizeY = m_SceneViewportSize.y;
							m_GameViewportSizeX = m_GameViewportSizeY / 9.0f * 16.0f;
						}
						break;
					case SceneCamera::AspectRatioType::SixteenByTen:
						m_GameViewportSizeX = m_SceneViewportSize.x;
						m_GameViewportSizeY = m_GameViewportSizeX / 16.0f * 10.0f;

						if (m_GameViewportSizeY > m_SceneViewportSize.y)
						{
							m_GameViewportSizeY = m_SceneViewportSize.y;
							m_GameViewportSizeX = m_GameViewportSizeY / 10.0f * 16.0f;
						}
						break;
					case SceneCamera::AspectRatioType::FourByThree:
						m_GameViewportSizeX = m_SceneViewportSize.x;
						m_GameViewportSizeY = m_GameViewportSizeX / 4.0f * 3.0f;

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
		if (entity && m_Gizmos->GetType() != GizmoType::NONE)
		{
			// Gizmos
			ImGuizmo::SetDrawlist();
			ImGuizmo::SetRect(m_SceneViewportBounds[0].x, m_SceneViewportBounds[0].y, m_SceneViewportBounds[1].x - m_SceneViewportBounds[0].x, m_SceneViewportBounds[1].y - m_SceneViewportBounds[0].y);
			ImGuizmo::SetOrthographic(m_EditorCamera.GetProjectionType() == ProjectionType::Orthographic);

			auto& tc = entity.GetComponent<TransformComponent>();
			glm::mat4 transform = tc.GetTransform();
			
			const glm::mat4& cameraProjection = m_EditorCamera.GetProjection();
			const glm::mat4& cameraView = m_EditorCamera.GetViewMatrix();

			ImGuizmo::OPERATION operation = (ImGuizmo::OPERATION)0;

			float snapValue = 0.5f;
			switch (m_Gizmos->GetType())
			{
				case GizmoType::TRANSLATE:
					operation = ImGuizmo::OPERATION::TRANSLATE;
					break;
				case GizmoType::ROTATE:
					operation = ImGuizmo::OPERATION::ROTATE;
					snapValue = 15.0f;
					break;
				case GizmoType::SCALE:
					operation = ImGuizmo::OPERATION::SCALE;
					break;
				case GizmoType::BOUNDARY:
					operation = ImGuizmo::OPERATION::BOUNDS;
					snapValue = 0.1f;
					break;
			}

			float snapValues[] = { snapValue, snapValue, snapValue };
			float bounds[] = { -0.5f, -0.5f, -0.5f, 0.5f, 0.5f, 0.5f };
			bool boundSizing = m_Gizmos->GetType() == GizmoType::BOUNDARY
				&& !entity.HasComponent<CameraComponent>()
				&& !entity.HasComponent<LightComponent>()
				&& !entity.HasComponent<AudioComponent>();

			bool snap = Input::IsKeyPressed(KeyCode(Key::LeftShift));
			ImGuizmo::Manipulate(glm::value_ptr(cameraView), glm::value_ptr(cameraProjection), operation, static_cast<ImGuizmo::MODE>(m_GizmosMode),
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

			if (ImGui::IsWindowFocused() && Input::IsKeyPressed(Key::Escape))
			{
				m_Gizmos->SetType(GizmoType::NONE);
			}
		}

		ImGui::End();
		ImGui::PopStyleVar();
	}

	void EditorLayer::SceneViewportToolbar()
	{
		ImGuiWindowFlags window_flags = ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse;
		if (ImGui::Begin("Toolbar", nullptr, window_flags))
		{
			const auto canvasPos = ImGui::GetCursorScreenPos();
			const auto canvasSize = ImGui::GetContentRegionAvail();

			ImDrawList* drawList = ImGui::GetWindowDrawList();

			ImVec2 posMinA = ImVec2(canvasPos);
			ImVec2 posMaxA = ImVec2(canvasPos.x + 600.0f, canvasPos.y + canvasSize.y);

			/*static float hue = 0.0f;
			hue += ImGui::GetIO().DeltaTime * 0.1f;
			if (hue >= 360.0f)
				hue -= 360.0f;
			uint32_t rectColor = (ImU32)ImColor::HSV(hue, 0.5f, 1.0f);*/
			uint32_t rectColor = ImColor(0.2231f, 0.44321f, 0.1f);
			if (m_SceneState == SceneState::Play)
				rectColor = ImColor(0.7213f, 0.2321f, 0.1f);

			uint32_t rectTransparentColor = ImColor(0.0f, 0.0f, 0.0f, 0.0f);
			drawList->AddRectFilledMultiColor(posMinA, posMaxA, rectColor, rectTransparentColor, rectTransparentColor, rectColor);
			ImVec2 posMinB = ImVec2(canvasPos.x + canvasSize.x - 600.0f, canvasPos.y);
			ImVec2 posMaxB = ImVec2(canvasPos.x + canvasSize.x, canvasPos.y + canvasSize.y);
			drawList->AddRectFilledMultiColor(posMinB, posMaxB, rectTransparentColor, rectColor, rectColor, rectTransparentColor);

			ImTextureID origiEngineTex = reinterpret_cast<ImTextureID>(m_OriginEngineTex->GetRendererID());
			float textureAspect = (float)m_OriginEngineTex->GetWidth() / (float)m_OriginEngineTex->GetHeight();

			ImVec2 imagePosMin = ImVec2(canvasPos.x, canvasPos.y);
			ImVec2 imagePosMax = ImVec2(canvasPos.x + (canvasSize.y * textureAspect), canvasPos.y + canvasSize.y);
			drawList->AddImage(origiEngineTex, imagePosMin, imagePosMax, 
				{ 0.0f, 1.0f }, { 1.0f, 0.0f });
			
			ImVec2 btPos = { (canvasSize.x / 2.0f) - 175.0f, (canvasSize.y - 30.0f) / 2.0f };
			ImGui::SetCursorPos(btPos);

			ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0, 0, 0, 0));
			ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.3f, 0.3f, 0.3f, 0.3f));
			ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0, 0, 0, 0));
			// Play Button
			std::shared_ptr<Texture2D> icon = (m_SceneState == SceneState::Edit || m_SceneState == SceneState::Simulate) ? m_UITextures.at("play") : m_UITextures.at("stop");
			if (ImGui::ImageButton((ImTextureID)icon->GetRendererID(), { 25.0f, 25.0f }))
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

			// Simulate Button
			ImGui::SameLine();
			bool isNotSimulate = m_SceneState == SceneState::Edit || m_SceneState == SceneState::Play;
			icon = isNotSimulate ? m_UITextures.at("simulate") : m_UITextures.at("stop");
			if (ImGui::ImageButton((ImTextureID)icon->GetRendererID(), {25.0f, 25.0f}))
			{
				if (m_SceneHierarchy.GetContext())
				{
					if (m_SceneState == SceneState::Edit || m_SceneState == SceneState::Play)
						OnSceneSimulate();
					else if (m_SceneState == SceneState::Simulate)
						OnSceneStop();
				}
			}

			// Pause Button
			if (m_SceneState != SceneState::Edit)
			{
				ImGui::SameLine();
				bool isPaused = m_ActiveScene->IsPaused();
				icon = m_UITextures.at("pause");
				if (ImGui::ImageButton((ImTextureID)icon->GetRendererID(), { 25.0f, 25.0f }))
				{
					m_ActiveScene->SetPaused(!isPaused);
				}

				if (isPaused)
				{
					icon = m_UITextures.at("stepping");
					ImGui::SameLine();
					if (ImGui::ImageButton((ImTextureID)icon->GetRendererID(), { 25.0f, 25.0f }))
					{
						m_ActiveScene->Step(1);
					}
				}
			}

			ImGui::SameLine();

			// Projection mode
			ImGui::SetCursorPos({ btPos.x + 175.0f, btPos.y });
			const auto& mode = m_EditorCamera.GetProjectionType();
			icon = mode == ProjectionType::Orthographic ? m_UITextures.at("camera_2d_projection") : m_UITextures.at("camera_3d_projection");
			if (ImGui::ImageButton(reinterpret_cast<ImTextureID>(icon->GetRendererID()), ImVec2(25.0f, 25.0f), ImVec2(0, 1), ImVec2(1, 0)))
			{
				if (mode == ProjectionType::Perspective)
					m_EditorCamera.SetProjectionType(ProjectionType::Orthographic);
				else if (mode == ProjectionType::Orthographic)
					m_EditorCamera.SetProjectionType(ProjectionType::Perspective);
			}
			ImGui::PopStyleColor(3);

			ImGui::SetCursorPos({ canvasSize.x - 200.0f, btPos.y + 10.0f});
			ImGui::Text("%.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
			ImGui::End(); // !Toolbar
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
						Entity entity = EntityManager::CreateEntity("Emtpy", m_SceneHierarchy.GetContext().get());
						m_SceneHierarchy.SetSelectedEntity(entity);
					}

					if (ImGui::MenuItem("Main Camera"))
					{
						Entity entity = EntityManager::CreateCamera("Main Camera", m_SceneHierarchy.GetContext().get());
						entity.AddComponent<AudioListenerComponent>();

						m_SceneHierarchy.SetSelectedEntity(entity);
					}

					if (ImGui::MenuItem("Camera"))
					{
						Entity entity = EntityManager::CreateCamera("Camera", m_SceneHierarchy.GetContext().get());
						m_SceneHierarchy.SetSelectedEntity(entity);
					}
					ImGui::Separator();

					if (ImGui::MenuItem("Sprite"))
					{
						Entity entity = EntityManager::CreateEntity("Sprite", m_SceneHierarchy.GetContext().get());
						entity.AddComponent<SpriteRenderer2DComponent>();
						m_SceneHierarchy.SetSelectedEntity(entity);
					}
					ImGui::Separator();

					if (ImGui::MenuItem("Lighting"))
					{
						Entity entity = EntityManager::CreateEntity("Lighting", m_SceneHierarchy.GetContext().get());
						entity.AddComponent<LightComponent>().Light = Lighting::Create(LightingType::Directional);
						m_SceneHierarchy.SetSelectedEntity(entity);
					}

					if (ImGui::MenuItem("Empty Mesh"))
					{
						Entity entity = EntityManager::CreateMesh("Emtpy Mesh", m_SceneHierarchy.GetContext().get());
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

	void EditorLayer::GUIRender()
	{
		if (Entity entity = m_SceneHierarchy.GetSelectedEntity())
		{
			if (entity.HasComponent<AnimationComponent>())
			{
				auto& ac = entity.GetComponent<AnimationComponent>();
				m_AnimationTimeline.DrawEntityAnimation(ac.Animations, &ac.CurrentAnimation);
			}
		}
		
		if (guiRenderSettingsWindow)
		{
			ImGui::Begin("Render Settings", &guiRenderSettingsWindow);
			const char *CMSTypeString[] = { "PIVOT", "FREE MOVE" };
			const char *currentCMSTypeString = CMSTypeString[static_cast<int>(m_EditorCamera.GetStyle())];
			ImGui::Text("Distance %.5f", m_EditorCamera.GetDistance());

			if (ImGui::BeginCombo("CAMERA STYLE", currentCMSTypeString))
			{
				for (int i = 0; i < 2; i++)
				{
					const bool isSelected = currentCMSTypeString == CMSTypeString[i];
					if (ImGui::Selectable(CMSTypeString[i], isSelected))
					{
						currentCMSTypeString = CMSTypeString[i];
						m_EditorCamera.SetStyle(static_cast<CameraStyle>(i));

					} if (isSelected) ImGui::SetItemDefaultFocus();
				} ImGui::EndCombo();
			}
			if (ImGui::DragFloat("FOV", &m_CameraFov, 1.0f, 0.0f, 90.0f))
				m_EditorCamera.SetFov(m_CameraFov);

			ImGui::Checkbox("Visualize Collider", &m_VisualizeCollider);

			ImGui::Separator();

			const auto renderStats = Renderer::GetStatistics();
			ImGui::Text("Total Time (s) : (%.2f s)", m_Time);
			if (ImGui::Button("Reset Time")) { m_Time = 0.0f; }
			ImGui::Text("Draw Calls: %d", renderStats.DrawCalls);
			ImGui::Text("Quads: %d", renderStats.QuadCount);
			ImGui::Text("Circles: %d", renderStats.CircleCount);
			ImGui::Text("Lines: %d", renderStats.LineCount);
			ImGui::Text("Cubes: %d", renderStats.CubeCount);
			ImGui::Text("Vertices: %d", renderStats.GetTotalVertexCount());
			ImGui::Text("Indices: %d", renderStats.GetTotalIndexCount());
			ImGui::Text("OpenGL Version: (%s)", glGetString(GL_VERSION));
			ImGui::Text("ImGui version: (%s)", IMGUI_VERSION);
			ImGui::Text("ImGuizmo Hovered (%d)", ImGuizmo::IsOver());
			ImGui::Text("Viewport Hovered (%d)", m_SceneViewportHovered);
			ImGui::Text("Hovered Pixel (%d)", m_PixelData);
			ImGui::Separator();

			ImGui::SameLine(0.0f, 1.5f); ImGui::ColorEdit4("Background Color", glm::value_ptr(clearColor));

			const char *RTTypeString[] = { "Normal", "HDR" };
			const char *currentRTTypeString = RTTypeString[m_RenderTarget];

			if (ImGui::BeginCombo("Render Target", currentRTTypeString))
			{
				for (int i = 0; i < 2; i++)
				{
					const bool isSelected = currentRTTypeString == RTTypeString[i];
					if (ImGui::Selectable(RTTypeString[i], isSelected))
					{
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

		if (guiImGuiDemoWindow)
		{
			ImGui::ShowDemoWindow(&guiImGuiDemoWindow);
		}
	}

	bool EditorLayer::OnMouseButtonPressed(MouseButtonPressedEvent& e)
	{
		bool control = Input::IsKeyPressed(Key::LeftControl);

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
					if (m_PixelData >= -1)
					{
						m_SceneHierarchy.SetSelectedEntity({});
						m_Gizmos->SetType(GizmoType::NONE);
					}
				}
			}
		}
		
		return false;
	}

	void EditorLayer::InputProcedure(Timestep time)
	{
		static glm::vec2 initialPosition = { 0.0f, 0.0f };
		const glm::vec2 mouse { Input::GetMouseX(), Input::GetMouseY() };
		const glm::vec2 delta = mouse - initialPosition;
		initialPosition = mouse;

		Entity entity = m_SceneHierarchy.GetSelectedEntity();

		if (Input::IsMouseButtonPressed(Mouse::ButtonLeft))
		{
			float orthoScale = m_EditorCamera.GetOrthoSize() / m_EditorCamera.GetHeight();

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
						if (Input::IsKeyPressed(Key::LeftShift))
						{
							translate.x += delta.x * orthoScale;
							translate.y -= delta.y * orthoScale;

							if (Input::IsKeyPressed(Key::LeftControl))
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

		if (Input::IsMouseButtonPressed(Mouse::ButtonRight))
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
		const bool control = Input::IsKeyPressed(Key::LeftControl) || Input::IsKeyPressed(Key::RightControl);
		const bool shift = Input::IsKeyPressed(Key::LeftShift) || Input::IsKeyPressed(Key::RightShift);

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
					m_Gizmos->SetType(GizmoType::SCALE);
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
					m_Gizmos->SetType(GizmoType::ROTATE);
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
					m_Gizmos->SetType(GizmoType::TRANSLATE);
				break;
			}

			case Key::Y:
			{
				if (!ImGuizmo::IsUsing() && !io.WantTextInput)
					m_Gizmos->SetType(GizmoType::BOUNDARY2D);
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
