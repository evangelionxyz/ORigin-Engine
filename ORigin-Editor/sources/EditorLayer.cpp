
// Copyright (c) Evangelion Manuhutu | ORigin Engine

#include "EditorLayer.h"
#include "Panels/ModelLoaderPanel.h"

#include "Origin/EntryPoint.h"
#include "Gizmos/Gizmos.h"
#include "Origin/Utils/PlatformUtils.h"
#include "Origin/Scripting/ScriptEngine.h"
#include "Origin/Asset/AssetManager.h"
#include "Origin/Asset/AssetImporter.h"
#include "Origin/Scene/EntityManager.h"
#include "Panels/AnimationTimeline.h"
#include "Origin/GUI/UI.h"

#include <filesystem>
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/quaternion.hpp>
#include <glm/gtx/compatibility.hpp>

namespace origin
{
	static EditorLayer *s_Instance = nullptr;

	EditorLayer::EditorLayer() : Layer("EditorLayer") { s_Instance = this; }

	EditorLayer::~EditorLayer() { ScriptEngine::Shutdown(); }

	void EditorLayer::OnAttach()
	{
		OGN_PROFILER_FUNCTION();

		// Load UI Textures
		m_UITextures["play"] = TextureImporter::LoadTexture2D("Resources/UITextures/ic/ic_play.png");
		m_UITextures["simulate"] = TextureImporter::LoadTexture2D("Resources/UITextures/ic/ic_simulate.png");
		m_UITextures["stop"] = TextureImporter::LoadTexture2D("Resources/UITextures/ic/ic_stop.png");
		m_UITextures["pause"] = TextureImporter::LoadTexture2D("Resources/UITextures/ic/ic_pause.png");
		m_UITextures["stepping"] = TextureImporter::LoadTexture2D("Resources/UITextures/ic/ic_stepping.png");
		m_UITextures["plus"] = TextureImporter::LoadTexture2D("Resources/UITextures/ic/ic_plus.png");
		m_UITextures["eyes_open"] = TextureImporter::LoadTexture2D("Resources/UITextures/ic/ic_eyes_open.png");
		m_UITextures["eyes_closed"] = TextureImporter::LoadTexture2D("Resources/UITextures/ic/ic_eyes_closed.png");

		// Gizmo icons
		m_UITextures["audio"] = TextureImporter::LoadTexture2D("Resources/UITextures/audio.png");
		m_UITextures["camera"] = TextureImporter::LoadTexture2D("Resources/UITextures/camera.png");
		m_UITextures["lighting"] = TextureImporter::LoadTexture2D("Resources/UITextures/lighting.png");

		m_UITextures["camera_2d_projection"] = TextureImporter::LoadTexture2D("Resources/UITextures/camera_projection_2d_icon.png");
		m_UITextures["camera_3d_projection"] = TextureImporter::LoadTexture2D("Resources/UITextures/camera_projection_3d_icon.png");
		m_OriginEngineTex = TextureImporter::LoadTexture2D("Resources/UITextures/origin_engine.png");

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

		m_EditorCamera.InitPerspective(45.0f, 1.776f, 0.1f, 500.0f);
		m_EditorCamera.SetPosition({ 35.0f, 35.0f, 35.0f });
		m_EditorCamera.SetDistance(58.0f);
		m_EditorCamera.SetYaw(-0.7f);
		m_EditorCamera.SetPitch(0.63f);


		m_ActiveScene = std::make_shared<Scene>();
		const auto &commandLineArgs = Application::Get().GetSpecification().CommandLineArgs;

		if (commandLineArgs.Count > 1)
		{
			m_ProjectDirectoryPath = commandLineArgs[1];
			if (!OpenProject(m_ProjectDirectoryPath))
				Application::Get().Close();
		}

		m_SpriteSheetEditor = std::make_unique<SpriteSheetEditor>();
		m_Gizmos = std::make_unique<Gizmos>();

		if (!m_UIEditor)
		{
			m_UIEditor = std::make_unique<UIEditor>(m_ActiveScene.get());
		}
	}

	void EditorLayer::OnEvent(Event &e)
	{
		OGN_PROFILER_INPUT();

		EventDispatcher dispatcher(e);
		dispatcher.Dispatch<KeyPressedEvent>(OGN_BIND_EVENT_FN(EditorLayer::OnKeyPressed));
		dispatcher.Dispatch<MouseButtonPressedEvent>(OGN_BIND_EVENT_FN(EditorLayer::OnMouseButtonPressed));

		m_Gizmos->OnEvent(e);
		m_EditorCamera.OnEvent(e);
		m_SpriteSheetEditor->OnEvent(e);
		m_UIEditor->OnEvent(e);
	}

	void EditorLayer::OnUpdate(Timestep ts)
	{
		OGN_PROFILER_FUNCTION();

		ProfilerTimer timer("EditorLayer::OnUpdate", [&](ProfilerResult result)
		{
			m_ProfilerResults.push_back(result);
		});

		switch (m_SceneState)
		{
		case SceneState::Edit:
		case SceneState::Simulate:
		{
			const auto &fbSpec = m_Framebuffer->GetSpecification();
			if (m_SceneViewportSize.x != fbSpec.Width || m_SceneViewportSize.y != fbSpec.Height)
			{
				if (m_SceneViewportSize.x > 0.0f && m_SceneViewportSize.y > 0.0f)
				{
					m_Framebuffer->Resize(static_cast<uint32_t>(m_SceneViewportSize.x), static_cast<uint32_t>(m_SceneViewportSize.y));
					m_EditorCamera.SetViewportSize(m_SceneViewportSize.x, m_SceneViewportSize.y);
					m_ActiveScene->OnViewportResize(static_cast<uint32_t>(m_SceneViewportSize.x), static_cast<uint32_t>(m_SceneViewportSize.y));
				}

			}
			break;
		}
		case SceneState::Play:
			uint32_t width = m_ActiveScene->GetWidth();
			uint32_t height = m_ActiveScene->GetHeight();
			if (m_SceneViewportSize.x != width && m_SceneViewportSize.y != height)
			{
				if (m_SceneViewportSize.x > 0.0f && m_SceneViewportSize.y > 0.0f)
				{
					m_ActiveScene->OnViewportResize(static_cast<uint32_t>(m_SceneViewportSize.x), static_cast<uint32_t>(m_SceneViewportSize.y));
				}
				break;
			}
		}

		SystemUpdate(ts);

		// m_ActiveScene->OnShadowRender();
		m_ActiveScene->GetUIRenderer()->RenderFramebuffer();

		m_Framebuffer->Bind();
		RenderCommand::ClearColor(m_ClearColor);
		RenderCommand::Clear();
		m_Framebuffer->ClearAttachment(1, -1);

		Render(ts);
		m_ActiveScene->GetUIRenderer()->Render();

		if (IsViewportHovered && IsViewportFocused)
		{
			auto [mx, my] = ImGui::GetMousePos();
			m_ViewportMousePos = { mx, my };
			m_ViewportMousePos -= m_SceneViewportBounds[0];
			m_ViewportMousePos.y = m_SceneViewportSize.y - m_ViewportMousePos.y;
			m_ViewportMousePos = glm::clamp(m_ViewportMousePos, { 0.0f, 0.0f }, m_SceneViewportSize - glm::vec2(1.0f, 1.0f));

            m_PixelData = m_Framebuffer->ReadPixel(1, m_ViewportMousePos.x, m_ViewportMousePos.y);
            m_HoveredEntity = m_PixelData == -1 ? Entity() : Entity(static_cast<entt::entity>(m_PixelData), m_ActiveScene.get());
            m_Gizmos->SetHovered(m_PixelData);
		}

		InputProcedure(ts);
		m_Framebuffer->Unbind();
	}

	EditorLayer &EditorLayer::Get()
	{
		return *s_Instance;
	}

	void EditorLayer::SystemUpdate(Timestep ts)
	{
		m_Time += ts.Seconds();

		Renderer::GetStatistics().Reset();
		Application::Get().GetGuiLayer()->BlockEvents(!IsViewportFocused && !m_SpriteSheetEditor->IsViewportFocused && !m_UIEditor->IsViewportFocused);
		
		m_SpriteSheetEditor->OnUpdate(ts);
		m_UIEditor->OnUpdate(ts);

		if (m_SceneState != SceneState::Play)
		{
			m_EditorCamera.SetMoveActive(!ImGui::GetIO().WantTextInput && IsViewportFocused);
			m_EditorCamera.SetDraggingActive(IsViewportFocused && !m_SpriteSheetEditor->IsViewportFocused);
			m_EditorCamera.SetScrollingActive(IsViewportHovered);
		}
	}

	void EditorLayer::OnDuplicateEntity()
	{
		if (m_SceneState != SceneState::Edit)
			return;

		Entity selectedEntity = m_SceneHierarchy.GetSelectedEntity();
		if (selectedEntity.IsValid())
		{
			Entity entity = EntityManager::DuplicateEntity(selectedEntity, m_EditorScene.get());
			m_SceneHierarchy.SetSelectedEntity(entity);
		}
	}

	void EditorLayer::OnDestroyEntity()
	{
		if (m_SceneState != SceneState::Edit)
			return;

		Entity selectedEntity = m_SceneHierarchy.GetSelectedEntity();
		if (selectedEntity.IsValid())
		{
			m_ActiveScene->DestroyEntity(selectedEntity);
			m_SceneHierarchy.SetSelectedEntity({ });
		}
	}

	void EditorLayer::OnGuiRender()
	{
		m_Dockspace.Begin();
		m_UIEditor->OnImGuiRender();
		m_SpriteSheetEditor->OnImGuiRender();
		m_MaterialEditor.OnImGuiRender();
		m_SceneHierarchy.OnImGuiRender();
		ConsoleWindow();
		MenuBar();
		SceneViewportToolbar();
		GUIRender();
		ModelLoaderPanel::OnUIRender();
		SceneViewport();
		if (m_ContentBrowser) m_ContentBrowser->OnImGuiRender();
		m_Dockspace.End();
	}

	void EditorLayer::OnScenePlay()
	{
		OGN_PROFILER_SCENE();

		if (m_SceneState == SceneState::Simulate)
			OnSceneStop();

		m_SceneState = SceneState::Play;

		m_ActiveScene = Scene::Copy(m_EditorScene);
		m_ActiveScene->OnRuntimeStart();
		m_SceneHierarchy.SetActiveScene(m_ActiveScene);
	}

	void EditorLayer::OnScenePause()
	{
		OGN_PROFILER_SCENE();

		if (m_SceneState == SceneState::Edit)
			return;

		m_ActiveScene->SetPaused(true);
	}

	void EditorLayer::OnSceneSimulate()
	{
		OGN_PROFILER_RENDERING();

		if (m_SceneState == SceneState::Play)
			OnSceneStop();

		m_SceneState = SceneState::Simulate;

		m_ActiveScene = Scene::Copy(m_EditorScene);
		m_ActiveScene->OnSimulationStart();

		m_SceneHierarchy.SetActiveScene(m_ActiveScene);
	}

	void EditorLayer::OnSceneStop()
	{
		OGN_PROFILER_SCENE();

		ScriptEngine::ClearSceneContext();

		if (m_SceneState == SceneState::Play)
			m_ActiveScene->OnRuntimeStop();
		else if (m_SceneState == SceneState::Simulate)
			m_ActiveScene->OnSimulationStop();

		m_SceneHierarchy.SetActiveScene(m_EditorScene);
		m_ActiveScene = m_EditorScene;

		m_SceneState = SceneState::Edit;
	}

	bool EditorLayer::NewProject()
	{
		OGN_PROFILER_FUNCTION();

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
		OGN_PROFILER_FUNCTION();

		if (Project::Load(path))
		{
			ScriptEngine::Init();

			AssetHandle handle = Project::GetActive()->GetConfig().StartScene;
			OpenScene(handle);

			m_ProjectDirectoryPath = Project::GetActiveProjectDirectory();

			m_ContentBrowser = std::make_unique<ContentBrowserPanel>(Project::GetActive());
			if (!m_ContentBrowser)
			{
				OGN_CORE_ERROR("[EditorLayer] ContentBrowserPanel Failed to initialized");
				return false;
			}

			return true;
		}

		return false;
	}

	bool EditorLayer::OpenProject()
	{
		OGN_PROFILER_FUNCTION();

		if (Project::Open())
		{
			ScriptEngine::Init();

			AssetHandle handle = Project::GetActive()->GetConfig().StartScene;
			OpenScene(handle);

			m_ProjectDirectoryPath = Project::GetActiveProjectDirectory();

			m_ContentBrowser = std::make_unique<ContentBrowserPanel>(Project::GetActive());
			if (!m_ContentBrowser)
			{
				OGN_CORE_ERROR("[EditorLayer] ContentBrowserPanel Failed to initialized");
				return false;
			}

			return true;
		}

		return false;
	}
	
	void EditorLayer::NewScene()
	{
		OGN_PROFILER_SCENE();
		if (m_SceneState == SceneState::Play || m_SceneState == SceneState::Simulate)
			OnSceneStop();

		m_EditorScene = std::make_shared<Scene>();
		m_SceneHierarchy.SetActiveScene(m_EditorScene, true);
		m_ActiveScene = m_EditorScene;
		m_ScenePath = std::filesystem::path();
	}

	void EditorLayer::SaveScene()
	{
		OGN_PROFILER_SCENE();

		if (m_SceneState == SceneState::Play || m_SceneState == SceneState::Simulate)
			OnSceneStop();

		if (!m_ScenePath.empty())
			SerializeScene(m_ActiveScene, m_ScenePath);
		else
			SaveSceneAs();

		Project::GetActive()->GetEditorAssetManager()->SerializeAssetRegistry();
	}

	void EditorLayer::SaveSceneAs()
	{
		OGN_PROFILER_SCENE();
		if (m_SceneState == SceneState::Play || m_SceneState == SceneState::Simulate)
			OnSceneStop();
#ifdef _WIN32
		std::filesystem::path filepath = FileDialogs::SaveFile("ORigin Scene (*.org)\0*.org\0");
#elif __linux__
		std::filesystem::path filepath = FileDialogs::SaveFile("ORigin Scene | *.org");
#endif

		if (!filepath.empty())
		{
			OGN_CORE_INFO("[Scene Scene As]{0}", filepath);
			SerializeScene(m_ActiveScene, filepath);
			m_ScenePath = filepath;
		}
	}

	void EditorLayer::OpenScene(AssetHandle handle)
	{
		OGN_PROFILER_SCENE();

		if (!AssetManager::IsAssetHandleValid(handle) || handle == 0)
		{
			NewScene();
			return;
		}

		if (m_SceneState != SceneState::Edit)
		{
			OnSceneStop();	
		}

		auto metadata = Project::GetActive()->GetEditorAssetManager()->GetMetadata(handle);
		std::shared_ptr<Scene> readOnlyScene = AssetManager::GetAsset<Scene>(handle);
		

		if (!readOnlyScene)
		{
			OGN_CORE_ERROR("[EditorLayer] Invalid Scene");
			return;
		}

		m_EditorScene = Scene::Copy(readOnlyScene);
		m_HoveredEntity = {};

		m_SceneHierarchy.SetActiveScene(m_EditorScene, true);
		m_ActiveScene = m_EditorScene;
		m_ScenePath = metadata.Filepath;
		std::string name = metadata.Filepath.stem().string();
		m_ActiveScene->SetName(name);

		if (!m_UIEditor)
		{
			m_UIEditor = std::make_unique<UIEditor>(m_ActiveScene.get());
		}

		m_UIEditor->SetContext(m_ActiveScene.get());
	}

	void EditorLayer::OpenScene()
	{
		OGN_PROFILER_SCENE();

		if (m_SceneState == SceneState::Play)
			OnSceneStop();

#ifdef _WIN32
		std::filesystem::path filepath = FileDialogs::OpenFile("ORigin Scene (*.org)\0*.org\0");
#elif __linux__
		std::filesystem::path filepath = FileDialogs::OpenFile("ORigin Scene | *.org");
#endif

		if(filepath.empty())
			return;

		AssetHandle handle = SceneImporter::OpenScene(filepath);
		if (handle == 0 || filepath.empty())
			return;

		std::shared_ptr<Scene> readOnlyScene = AssetManager::GetAsset<Scene>(handle);
		std::string name = filepath.stem().string();
		readOnlyScene->SetName(name);

		if (!readOnlyScene)
		{
			OGN_CORE_ERROR("[EditorLayer] Invalid Scene");
			return;
		}

		m_HoveredEntity = {};
		m_EditorScene = Scene::Copy(readOnlyScene);
		m_SceneHierarchy.SetActiveScene(m_EditorScene, true);
		m_ActiveScene = m_EditorScene;

		m_ScenePath = filepath;

		if (!m_UIEditor)
			m_UIEditor = std::make_unique<UIEditor>(m_ActiveScene.get());

		m_UIEditor->SetContext(m_ActiveScene.get());
	}

	void EditorLayer::SerializeScene(const std::shared_ptr<Scene> &scene, const std::filesystem::path &filepath)
	{
		scene->SetName(filepath.stem().string());
		SceneImporter::SaveScene(scene, filepath);
	}

	void EditorLayer::MenuBar()
	{
		ImGuiStyle& style = ImGui::GetStyle();
		auto &window = Application::Get();
		ImGuiIO& io = ImGui::GetIO();

		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(8.0f, 8.0f));

		if (ImGui::BeginMenuBar())
		{
			ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(8.0f, 3.0f));

			if (ImGui::BeginMenu("File"))
			{
				if (ImGui::MenuItem("New Project")) NewProject();
				if (ImGui::MenuItem("Open Project")) OpenProject();
				if (ImGui::MenuItem("Save Project", "", nullptr, (bool)Project::GetActive())) Project::SaveActive();
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
				if (ImGui::MenuItem("Empty Entity", nullptr, nullptr, (bool)Project::GetActive()
					&& !m_ScenePath.empty())) m_SceneHierarchy.SetSelectedEntity(EntityManager::CreateEntityWithUUID(UUID(), "Empty", m_ActiveScene.get()));
				ImGui::EndMenu();
			}

			if (ImGui::BeginMenu("Window"))
			{
				ImGui::MenuItem("Style Editor", nullptr, &guiMenuStyle);
				ImGui::MenuItem("Render Settings", nullptr, &guiRenderSettingsWindow);
				ImGui::MenuItem("Demo Window", nullptr, &guiImGuiDemoWindow);

				ImGui::EndMenu();
			}

			if (ImGui::BeginMenu("View"))
			{
				if (ImGui::MenuItem("Full Screen", "F11", &guiMenuFullscreen))
					window.GetWindow().SetFullscreen(guiMenuFullscreen);
				ImGui::EndMenu();
			}

			ImGui::EndMenuBar();
			ImGui::PopStyleVar();
		}
		ImGui::PopStyleVar();
	}

	void EditorLayer::Render(Timestep ts)
	{
		OGN_PROFILER_RENDERING();

		ProfilerTimer timer("EditorLayer::Render", [&](ProfilerResult result)
		{
			m_ProfilerResults.push_back(result);
		});

		switch (m_SceneState)
		{
		case SceneState::Play:
			m_Gizmos->SetType(GizmoType::NONE);
			m_ActiveScene->OnUpdateRuntime(ts);
			break;

		case SceneState::Edit:
			if (m_Draw3DGrid)
				m_Gizmos->Draw3DGrid(m_EditorCamera, true, false, m_3DGridSize);
			if (m_Draw2DGrid)
				m_Gizmos->Draw2DGrid(m_EditorCamera);

			m_EditorCamera.OnUpdate(ts);
			m_ActiveScene->OnEditorUpdate(ts, m_EditorCamera);
			m_Gizmos->OnRender(m_EditorCamera);
			break;

		case SceneState::Simulate:
			if (m_Draw3DGrid)
				m_Gizmos->Draw3DGrid(m_EditorCamera, true, false, m_3DGridSize);
			if (m_Draw2DGrid)
				m_Gizmos->Draw2DGrid(m_EditorCamera);

			m_EditorCamera.OnUpdate(ts);
			m_ActiveScene->OnUpdateSimulation(ts, m_EditorCamera);
			m_Gizmos->OnRender(m_EditorCamera);
			break;
		}
	}

	void EditorLayer::SceneViewport()
	{
		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));
		ImGuiWindowFlags window_flags = ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse | ImGuiWindowFlags_NoCollapse;

		ImGui::Begin("Scene", nullptr, window_flags);
		
		IsViewportHovered = ImGui::IsWindowHovered();
		IsViewportFocused = ImGui::IsWindowFocused();
		
		const ImVec2& viewportMinRegion = ImGui::GetWindowContentRegionMin();
		const ImVec2& viewportMaxRegion = ImGui::GetWindowContentRegionMax();
		const ImVec2& viewportOffset = ImGui::GetWindowPos();
		m_SceneViewportBounds[0] = { viewportMinRegion.x + viewportOffset.x, viewportMinRegion.y + viewportOffset.y };
		m_SceneViewportBounds[1] = { viewportMaxRegion.x + viewportOffset.x, viewportMaxRegion.y + viewportOffset.y };

		ImVec2 viewportPanelSize = ImGui::GetContentRegionAvail();
		m_SceneViewportSize = { viewportPanelSize.x, viewportPanelSize.y };
		
		if (m_SceneState == SceneState::Play)
		{
			auto camView = m_ActiveScene->m_Registry.view<CameraComponent>();
			for (auto &e : camView)
			{
				auto &cc = camView.get<CameraComponent>(e);

				if (cc.Primary)
				{
					switch (cc.Camera.GetAspectRatioType())
					{
					case AspectRatioType::TwentyOneByNine:
						m_GameViewportSize.x = m_SceneViewportSize.x;
						m_GameViewportSize.y = m_GameViewportSize.x / 21.0f * 9.0f;

						if (m_GameViewportSize.y > m_SceneViewportSize.y)
						{
							m_GameViewportSize.y = m_SceneViewportSize.y;
							m_GameViewportSize.x = m_GameViewportSize.y / 9.0f * 21.0f;
						}
						break;
					case AspectRatioType::SixteenByNine:
						m_GameViewportSize.x = m_SceneViewportSize.x;
						m_GameViewportSize.y = m_GameViewportSize.x / 16.0f * 9.0f;

						if (m_GameViewportSize.y > m_SceneViewportSize.y)
						{
							m_GameViewportSize.y = m_SceneViewportSize.y;
							m_GameViewportSize.x = m_GameViewportSize.y / 9.0f * 16.0f;
						}
						break;
					case AspectRatioType::SixteenByTen:
						m_GameViewportSize.x = m_SceneViewportSize.x;
						m_GameViewportSize.y = m_GameViewportSize.x / 16.0f * 10.0f;

						if (m_GameViewportSize.y > m_SceneViewportSize.y)
						{
							m_GameViewportSize.y = m_SceneViewportSize.y;
							m_GameViewportSize.x = m_GameViewportSize.y / 10.0f * 16.0f;
						}
						break;
					case AspectRatioType::FourByThree:
						m_GameViewportSize.x = m_SceneViewportSize.x;
						m_GameViewportSize.y = m_GameViewportSize.x / 4.0f * 3.0f;

						if (m_GameViewportSize.y > m_SceneViewportSize.y)
						{
							m_GameViewportSize.y = m_SceneViewportSize.y;
							m_GameViewportSize.x = m_GameViewportSize.y / 3.0f * 4.0f;
						}
						break;
					case AspectRatioType::Free:
						m_GameViewportSize.x = m_SceneViewportSize.x;
						m_GameViewportSize.y = m_SceneViewportSize.y;
						break;
					}
				}
			}

			// centered image position
			float offsetX = (m_SceneViewportSize.x - m_GameViewportSize.x) * 0.5f;
			float offsetY = (m_SceneViewportSize.y - m_GameViewportSize.y) * 0.5f;
			ImGui::SetCursorPos({ offsetX, offsetY + 20.0f });
		}
		else
		{
			m_GameViewportSize.x = m_SceneViewportSize.x;
			m_GameViewportSize.y = m_SceneViewportSize.y;
		}

		ImTextureID viewportID = reinterpret_cast<ImTextureID>(m_Framebuffer->GetColorAttachmentRendererID(m_RenderTarget));
		ImGui::Image(viewportID, ImVec2(m_GameViewportSize.x, m_GameViewportSize.y), ImVec2(0, 1), ImVec2(1, 0));
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

		m_ImGuizmoOperation = ImGuizmo::OPERATION::NONE;

		float snapValue = 0.5f;
		switch (m_Gizmos->GetType())
		{
		case GizmoType::TRANSLATE:
			m_ImGuizmoOperation = ImGuizmo::OPERATION::TRANSLATE;
			break;
		case GizmoType::ROTATE:
			m_ImGuizmoOperation = ImGuizmo::OPERATION::ROTATE;
			snapValue = 15.0f;
			break;
		case GizmoType::SCALE:
			m_ImGuizmoOperation = ImGuizmo::OPERATION::SCALE;
			break;
		case GizmoType::BOUNDARY:
			m_ImGuizmoOperation = ImGuizmo::OPERATION::BOUNDS;
			snapValue = 0.1f;
			break;
		}

		Entity entity = m_SceneHierarchy.GetSelectedEntity();
		if (entity.IsValid() && m_Gizmos->GetType() != GizmoType::NONE)
		{
			// Gizmos
			ImGuizmo::SetDrawlist();
			ImGuizmo::SetRect(m_SceneViewportBounds[0].x, m_SceneViewportBounds[0].y, m_SceneViewportBounds[1].x - m_SceneViewportBounds[0].x, m_SceneViewportBounds[1].y - m_SceneViewportBounds[0].y);
			ImGuizmo::SetOrthographic(m_EditorCamera.GetProjectionType() == ProjectionType::Orthographic);

			auto &tc = entity.GetComponent<TransformComponent>();
			glm::mat4 transform = tc.GetTransform();
			auto &idc = entity.GetComponent<IDComponent>();
			
			const glm::mat4 &cameraProjection = m_EditorCamera.GetProjection();
			const glm::mat4 &cameraView = m_EditorCamera.GetViewMatrix();

			float snapValues[] = { snapValue, snapValue, snapValue };
			float bounds[] = { -0.5f, -0.5f, -0.5f, 0.5f, 0.5f, 0.5f };
			bool boundSizing = m_Gizmos->GetType() == GizmoType::BOUNDARY
				&& !entity.HasComponent<CameraComponent>()
				&& !entity.HasComponent<LightComponent>()
				&& !entity.HasComponent<AudioComponent>();

			bool snap = Input::IsKeyPressed(KeyCode(Key::LeftShift));
			ImGuizmo::Manipulate(glm::value_ptr(cameraView), glm::value_ptr(cameraProjection), m_ImGuizmoOperation, static_cast<ImGuizmo::MODE>(m_GizmosMode),
				glm::value_ptr(transform), nullptr, snap ? snapValues : nullptr, boundSizing ? bounds : nullptr, snap ? snapValues : nullptr);

			if (ImGuizmo::IsUsing())
			{
				glm::vec3 translation, rotation, scale;
				Math::DecomposeTransformEuler(transform, translation, rotation, scale);

				if (entity.HasParent())
				{
					auto &ptc = m_ActiveScene->GetEntityWithUUID(idc.Parent).GetComponent<TransformComponent>();
					glm::vec4 localTranslation = glm::inverse(ptc.GetTransform()) * glm::vec4(translation, 1.0f);

					// Apply parent's scale to local translation
					localTranslation.x *= ptc.WorldScale.x;
					localTranslation.y *= ptc.WorldScale.y;
					localTranslation.z *= ptc.WorldScale.z;

					// Convert back to world space
					tc.Translation = glm::vec3(ptc.GetTransform() * localTranslation);

					tc.Translation = glm::vec3(localTranslation);
					tc.Rotation = rotation - ptc.WorldRotation;
					tc.Scale = scale / ptc.WorldScale;
				}
				else
				{
					tc.Translation = translation;
					tc.Rotation += rotation - tc.Rotation;
					tc.Scale = scale;
				}
			}
		}

		if (ImGui::IsWindowFocused() && Input::IsKeyPressed(Key::Escape))
			m_Gizmos->SetType(GizmoType::NONE);

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
			const auto &mode = m_EditorCamera.GetProjectionType();
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

	void EditorLayer::GUIRender()
	{
		Entity entity = m_SceneHierarchy.GetSelectedEntity();
		if (entity.IsValid())
		{
			if (entity.HasComponent<SpriteAnimationComponent>())
			{
				auto &ac = entity.GetComponent<SpriteAnimationComponent>();
				AnimationTimeline::DrawSpriteAnimTimeline(ac);
			}
		}

		if (guiStatisticWindow)
		{
			ImGui::Begin("Statistics", &guiStatisticWindow);
			if (ImGui::BeginTabBar("##Statistics"))
			{
				if (ImGui::BeginTabItem("Render Settings"))
				{
					UI::DrawCheckbox("Visualize Collider", &m_VisualizeCollider);

					switch (m_EditorCamera.GetProjectionType())
					{
					case ProjectionType::Perspective:
						{
							const char *CMSTypeString[] = { "PIVOT", "FREE MOVE" };
							const char *currentCMSTypeString = CMSTypeString[static_cast<int>(m_EditorCamera.GetStyle())];
							ImGui::Text("Viewport Size %.0f, %.0f", m_SceneViewportSize.x, m_SceneViewportSize.y);
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

							float fov = m_EditorCamera.GetFOV();
							if (UI::DrawFloatControl("FOV", &fov, 1.0f, 20.0f, 120.0f))
								m_EditorCamera.SetFov(fov);

							UI::DrawCheckbox("Grid 3D", &m_Draw3DGrid);
							if (m_Draw3DGrid)
								UI::DrawIntControl("Grid Size", &m_3DGridSize, 1.0f);
							break;
						}
					case ProjectionType::Orthographic:
						UI::DrawCheckbox("Grid 2D", &m_Draw2DGrid);
						break;
					}

					ImGui::ColorEdit4("Background Color", glm::value_ptr(m_ClearColor));

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
							}
							if (isSelected) ImGui::SetItemDefaultFocus();
						}
						ImGui::EndCombo();
					}
					ImGui::EndTabItem();
				}

				ImGui::Separator();
				const auto renderStats = Renderer::GetStatistics();
				
				ImGui::Text("Draw Calls: %d", renderStats.DrawCalls);
				ImGui::Text("Quads: %d", renderStats.QuadCount);
				ImGui::Text("Circles: %d", renderStats.CircleCount);
				ImGui::Text("Lines: %d", renderStats.LineCount);
				ImGui::Text("Cubes: %d", renderStats.CubeCount);
				ImGui::Text("Vertices: %d", renderStats.GetTotalQuadVertexCount());
				ImGui::Text("Indices: %d", renderStats.GetTotalQuadIndexCount());
				ImGui::Text("OpenGL Version: (%s)", glGetString(GL_VERSION));
				ImGui::Text("ImGui version: (%s)", IMGUI_VERSION);
				ImGui::Text("ImGuizmo Hovered (%d)", ImGuizmo::IsOver());
				ImGui::Text("Viewport Hovered (%d)", IsViewportHovered);
				ImGui::Text("Hovered Pixel (%d)", m_PixelData);
				ImGui::Separator();

				if (ImGui::BeginTabItem("Render Time"))
				{
					int h = ((int)m_Time / 3600) % 60;
					int m = ((int)m_Time / 60) % 60;
					int s = (int)m_Time % 60;

					char timer[34];
					strcpy(timer, "Total Working Timer: %d : %d : %d");
					ImGui::Text(timer, h, m, s);

					if (ImGui::Button("Reset Time")) { 
						m_Time = 0.0f; 
					}

					for (auto r : m_ProfilerResults)
					{
						char label[50];
						strcpy(label, "[%.3fms]\t");
						strcat(label, r.Name);
						ImGui::Text(label, r.Duration);
					}

					m_ProfilerResults.clear();

					ImGui::EndTabItem();
				}
				ImGui::EndTabBar();
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

    void EditorLayer::ConsoleWindow()
    {
		ImGui::Begin("Console");
        ImGuiWindowFlags childFlags = ImGuiWindowFlags_NoScrollWithMouse | ImGuiWindowFlags_NoScrollbar;
        ImGui::BeginChild("navigation_button", ImVec2(ImGui::GetContentRegionAvail().x, 30.0f), false, childFlags);
		if (UI::DrawButton("CLEAR"))
		{
			ConsoleManager::Get().Clear();
		}
		ImGui::EndChild();

		ImGui::BeginChild("CONSOLE_CONTENT", ImGui::GetContentRegionAvail());

        // By default, if we don't enable ScrollX the sizing policy for each column is "Stretch"
        // All columns maintain a sizing weight, and they will occupy all available width.
        static ImGuiTableFlags flags = ImGuiTableFlags_Resizable | ImGuiTableFlags_BordersOuterH | ImGuiTableFlags_BordersInnerH
			| ImGuiTableFlags_BordersOuterV | ImGuiTableFlags_ContextMenuInBody | ImGuiTableFlags_RowBg;

        if (ImGui::BeginTable("table1", 3, flags))
        {
			float timestampWidth = ImGui::CalcTextSize("Timestamp").x + 2.0f;
            ImGui::TableSetupColumn("Timestamp");
            ImGui::TableSetupColumn("Message");
            ImGui::TableSetupColumn("Type");
            ImGui::TableHeadersRow();

            for (auto &m : ConsoleManager::GetMessages())
            {
                ImGui::TableNextRow();

                switch (m.Type)
                {
                case ConsoleMessageType::Info:    ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.0f, 1.0f, 1.0f, 1.0f)); break;
                case ConsoleMessageType::Error:   ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.8f, 0.0f, 0.0f, 1.0f)); break;
                case ConsoleMessageType::Warning: ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.2f, 0.5f, 0.2f, 1.0f)); break;
                }

				ImGui::TableSetColumnIndex(0);
				ImGui::Text("%s", m.Timestamp.c_str());
                ImGui::TableSetColumnIndex(1);
                ImGui::Text("%s", m.Message.c_str());
                ImGui::TableSetColumnIndex(2);

                switch (m.Type)
                {
                case ConsoleMessageType::Info:    ImGui::Text("INFO"); break;
                case ConsoleMessageType::Error:   ImGui::Text("ERROR"); break;
				case ConsoleMessageType::Warning: ImGui::Text("WARN"); break;
                }

				ImGui::PopStyleColor(1);
            }

            ImGui::EndTable();
        }

		ImGui::EndChild();

		ImGui::End();
    }

    bool EditorLayer::OnMouseButtonPressed(MouseButtonPressedEvent &e)
	{
		OGN_PROFILER_INPUT();
		
		return false;
	}

	void EditorLayer::InputProcedure(Timestep time)
	{
		if (m_SceneState == SceneState::Play)
			return;

		static glm::vec2 initialPosition = { 0.0f, 0.0f };
		const glm::vec2 mouse { Input::GetMouseX(), Input::GetMouseY() };
		const glm::vec2 delta = mouse - initialPosition;
		initialPosition = mouse;

		if (Input::IsMouseButtonPressed(Mouse::ButtonLeft))
		{
			Entity selectedEntity = m_SceneHierarchy.GetSelectedEntity();

            if (IsViewportHovered && !ImGui::IsMouseDragging(ImGuiMouseButton_Left) && 
				((m_Gizmos->GetType() == GizmoType::NONE) || !ImGuizmo::IsOver()))
			{
                if (m_PixelData >= 0)
                {
					if (m_HoveredEntity != selectedEntity)
					{
						m_SceneHierarchy.SetSelectedEntity(m_HoveredEntity);
						selectedEntity = m_HoveredEntity;
					}
                }
                else if (m_PixelData == -1)
                {
                    m_Gizmos->SetType(GizmoType::NONE);
                    m_SceneHierarchy.SetSelectedEntity({});
					selectedEntity = {};
                }
			}

			if (selectedEntity.IsValid() && IsViewportFocused && IsViewportHovered)
			{
				if (m_EditorCamera.GetProjectionType() == ProjectionType::Orthographic && !ImGuizmo::IsUsing())
				{
					auto &tc = selectedEntity.GetComponent<TransformComponent>();
					auto &idc = selectedEntity.GetComponent<IDComponent>();

					float orthoScale = m_EditorCamera.GetOrthoScale() / m_EditorCamera.GetHeight();
					if (m_SceneState == SceneState::Play)
					{
						Entity cam = m_ActiveScene->GetPrimaryCameraEntity();
						if (cam.IsValid())
						{
							CameraComponent &cc = cam.GetComponent<CameraComponent>();
							orthoScale = cc.Camera.GetOrthoScale() / cc.Camera.GetViewportSize().y;
						}
					}

					static glm::vec3 translation = tc.Translation;
					if (selectedEntity.HasComponent<Rigidbody2DComponent>() && m_SceneState != SceneState::Edit)
					{
						glm::vec3 velocity = glm::vec3(0.0f);
						auto &rb2d = selectedEntity.GetComponent<Rigidbody2DComponent>();
						auto body = static_cast<b2Body *>(rb2d.RuntimeBody);
						if (selectedEntity.HasParent())
						{
							Entity parent = m_ActiveScene->GetEntityWithUUID(idc.Parent);
							auto &parentTC = parent.GetComponent<TransformComponent>();
							velocity += glm::inverse(glm::quat(parentTC.WorldRotation)) * glm::vec3(delta.x * orthoScale * 0.5f, -delta.y * orthoScale * 0.5f, 0.0f);
						}
						else
						{
							velocity += glm::vec3(delta.x * orthoScale * 0.5f, -delta.y * orthoScale * 0.5f, 0.0f);
						}

						body->SetLinearVelocity(b2Vec2(velocity.x, velocity.y));
					}
					else
					{
						if (Input::IsKeyPressed(Key::LeftShift))
						{
							float snapeValue = 0.5f;
							if (Input::IsKeyPressed(Key::LeftControl))
								snapeValue = 0.1f;

							if (selectedEntity.HasParent())
							{
								Entity parent = m_ActiveScene->GetEntityWithUUID(idc.Parent);
								auto &parentTC = parent.GetComponent<TransformComponent>();

								translation += glm::inverse(glm::quat(parentTC.WorldRotation)) * glm::vec3(delta.x * orthoScale, -delta.y * orthoScale, 0.0f);
							}
							else
							{
								translation += glm::vec3(delta.x * orthoScale, -delta.y * orthoScale, 0.0f);
							}

							tc.Translation.x = round(translation.x / snapeValue) * snapeValue;
							tc.Translation.y = round(translation.y / snapeValue) * snapeValue;
						}
						else
						{
							translation = glm::vec3(delta.x * orthoScale, -delta.y * orthoScale, 0.0f);

							if (selectedEntity.HasParent())
							{
								Entity parent = m_ActiveScene->GetEntityWithUUID(idc.Parent);
								auto &parentTC = parent.GetComponent<TransformComponent>();
								translation = glm::inverse(glm::quat(parentTC.WorldRotation)) * translation;
							}
							tc.Translation += glm::vec3(glm::vec2(translation), 0.0f);
							translation = tc.Translation;
						}
					}
				}
			}
		}
	}

	bool EditorLayer::OnKeyPressed(KeyPressedEvent& e)
	{
		OGN_PROFILER_INPUT();

		auto &app = Application::Get();
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
				if (selectedEntity.IsValid() && !io.WantTextInput)
				{
					const auto &tc = selectedEntity.GetComponent<TransformComponent>();
					if (m_EditorCamera.GetProjectionType() == ProjectionType::Orthographic)
					{
						m_EditorCamera.SetPosition({ tc.WorldTranslation.x, tc.WorldTranslation.y, m_EditorCamera.GetPosition().z });
					}
					else
					{
						m_EditorCamera.SetPosition({ tc.WorldTranslation.x, tc.WorldTranslation.y, tc.WorldTranslation.z });
					}
					
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
				guiMenuFullscreen = !guiMenuFullscreen;
				app.GetWindow().SetFullscreen(guiMenuFullscreen);
				break;
			}

		case Key::Delete:
			{
				if(!io.WantTextInput)
					OnDestroyEntity();
				break;
			}
		}

		return false;
	}
}
