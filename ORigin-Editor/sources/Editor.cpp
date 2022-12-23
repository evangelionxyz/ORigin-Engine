// Copyright (c) 2022 Evangelion Manuhutu | ORigin Engine

#include "Editor.h"
#include "Origin/EntryPoint.h"
#include "panels/EditorTheme.h"
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "Origin/Utils/PlatformUtils.h"
#include "Origin/Scene/SceneSerializer.h"

#include <filesystem>
#include <ImGuizmo.h>

namespace Origin {
  extern const std::filesystem::path g_AssetPath;
  static float RMHoldTime = 0.0f;
  static float LMHoldTime = 0.0f;
  static bool guiDockingSpaceOpen = true;
  static bool guiMenuFullscreen = false;
  static bool guiMenuStyle = true;
  static bool guiRenderStatus = true;
  static bool guiDebugInfo = true;
  static bool guiImGuiDemoWindow = true;
  static bool guiOverlay = true;

  Editor::Editor() : Layer("Editor") { }

  void Editor::OnAttach()
  {
    EditorTheme::ApplyRayTek();

    m_PlayButton = Texture2D::Create("assets/textures/playbutton.png");
    m_StopButton = Texture2D::Create("assets/textures/stopbutton.png");

    FramebufferSpecification fbSpec;
    fbSpec.Attachments =
    {
      FramebufferTextureFormat::RGBA8,
      FramebufferTextureFormat::RED_INTEGER,
      FramebufferTextureFormat::Depth
    };

    fbSpec.Width = 1280;
    fbSpec.Height = 720;
    m_Framebuffer = Framebuffer::Create(fbSpec);

    m_ActiveScene = std::make_shared<Scene>();

    const auto commandLineArgs = Application::Get().GetCommandLineArgs();
    if (commandLineArgs.Count > 1)
    {
      const auto sceneFilePath = commandLineArgs[1];
      SceneSerializer serializer(m_ActiveScene);
      serializer.Deserialize(sceneFilePath);
    }

    m_EditorCamera = EditorCamera(30.0f, 1.778f, 0.1f, 1000.0f);
    m_SceneHierarchy.SetContext(m_ActiveScene);
    m_EditorScene = Scene::Copy(m_ActiveScene);
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

    // Resize
    if (
      const FramebufferSpecification spec = m_Framebuffer->GetSpecification();
      m_ViewportSize.x > 0.0f && m_ViewportSize.y > 0.0f &&
      (m_ViewportSize.x != spec.Width || m_ViewportSize.y != spec.Height)
      )
    {
      m_Framebuffer->Resize(static_cast<uint32_t>(m_ViewportSize.x), static_cast<uint32_t>(m_ViewportSize.y));
      m_EditorCamera.SetViewportSize(m_ViewportSize.x, m_ViewportSize.y);
      m_ActiveScene->OnViewportResize(static_cast<uint32_t>(m_ViewportSize.x), static_cast<uint32_t>(m_ViewportSize.y));
    }

    InputProccedure(time);

    m_Framebuffer->Bind();

    Renderer2D::ResetStats();
    RenderCommand::ClearColor(clearColor);
    RenderCommand::Clear();
    m_Framebuffer->ClearAttachment(1, -1);

    switch (m_SceneState)
    {
    case SceneState::Play:
      m_GizmosType = -1;
      m_ActiveScene->OnUpdateRuntime(time);
      break;

    case SceneState::Edit:
      m_EditorCamera.OnUpdate(time);
      m_ActiveScene->OnUpdateEditor(time, m_EditorCamera);
      break;
    }

    OnOverlayRenderer();

    auto [mx, my] = ImGui::GetMousePos();
    mx -= m_ViewportBounds[0].x;
    my -= m_ViewportBounds[0].y;
    const glm::vec2 viewportSize = m_ViewportBounds[1] - m_ViewportBounds[0];
    my = viewportSize.y - my;
    mouseX = static_cast<int>(mx);
    mouseY = static_cast<int>(my);

    if (mouseX >= 0 && mouseY >= 0 && mouseX < static_cast<int>(viewportSize.x) && mouseY < static_cast<int>(viewportSize.y))
    {
      m_PixelData = m_Framebuffer->ReadPixel(1, mouseX, mouseY);
      m_HoveredEntity = m_PixelData == -1 ? Entity() : Entity(static_cast<entt::entity>(m_PixelData), m_ActiveScene.get());
    }
    m_Framebuffer->Unbind();
  }

  void Editor::OnGuiRender()
  {
    m_Dockspace.Begin();

    VpGui();
    MenuBar();
    m_SceneHierarchy.OnImGuiRender();
    m_ContentBrowser.OnImGuiRender();
    m_Dockspace.End();
  }

	void Editor::OnOverlayRenderer()
	{
		if (m_SceneState == SceneState::Play)
		{
			Entity camera = m_ActiveScene->GetPrimaryCameraEntity();
      if (camera)
      {
				glm::mat4 transform = camera.GetComponent<TransformComponent>().GetTransform();
				Renderer2D::BeginScene(camera.GetComponent<CameraComponent>().Camera, transform);
      }
		}
		else Renderer2D::BeginScene(m_EditorCamera);

		// Draw selected entity outline 
		if (Entity selectedEntity = m_SceneHierarchy.GetSelectedEntity())
    {
			TransformComponent transform = selectedEntity.GetComponent<TransformComponent>();
			Renderer2D::DrawRect(transform.GetTransform(), glm::vec4(1, 0.5, 0, 1));
		}

    Renderer2D::EndScene();
	}

	void Editor::ViewportToolbar()
  {
    ImVec2& viewportMinRegion = ImGui::GetWindowContentRegionMin();
    ImVec2& viewportMaxRegion = ImGui::GetWindowContentRegionMax();
    ImVec2& viewportOffset = ImGui::GetWindowPos();

    const float& wndWidth = ImGui::GetWindowWidth();

    ImGuiWindowFlags window_flags = ImGuiWindowFlags_NoScrollbar
      | ImGuiWindowFlags_NoScrollWithMouse
      | ImGuiWindowFlags_AlwaysAutoResize
      | ImGuiWindowFlags_NoDecoration
      | ImGuiWindowFlags_NoCollapse
      | ImGuiWindowFlags_NoCollapse;

    float wndYpos = { (viewportMinRegion.y + viewportOffset.y) + 4.0f };

    // Play Button
    ImGui::SetNextWindowPos({ (viewportMinRegion.x + viewportOffset.x) + wndWidth / 2.0f, wndYpos }, ImGuiCond_Always);
    ImGui::SetNextWindowBgAlpha(0.0f);

    if (ImGui::Begin("##play_button", nullptr, window_flags))
    {
      ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 2.0f));
      ImGui::PushStyleVar(ImGuiStyleVar_ItemInnerSpacing, ImVec2(0.0f, 0.0f));

      std::shared_ptr<Texture2D> icon = m_SceneState == SceneState::Edit ? m_PlayButton : m_StopButton;

      ImGui::SameLine(viewportMinRegion.x * 0.5f);
      ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0, 0, 0, 0));
      ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.3f, 0.3f, 0.3f, 0.3f));
      ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0, 0, 0, 0));
      if (ImGui::ImageButton(reinterpret_cast<ImTextureID>(icon->GetRendererID()), ImVec2(25.0f, 25.0f)))
      {
        if (m_SceneState == SceneState::Edit)
          OnScenePlay();
        else if (m_SceneState == SceneState::Play)
          OnSceneStop();
      }
      ImGui::PopStyleColor(3);
      ImGui::PopStyleVar(2);

      ImGui::End(); // !viewport_toolbar
    }

    // Guizmo MODE
    ImGui::SetNextWindowPos({ (viewportMinRegion.x + viewportOffset.x) + wndWidth - 120.0f, wndYpos }, ImGuiCond_Always);
    ImGui::SetNextWindowBgAlpha(0.0f);
    if (ImGui::Begin("##gizmo_mode", nullptr, window_flags))
    {
      ImVec4 btActive = ImVec4(0.2f, 0.2f, 0.2f, 1.0f);
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

  const char* Editor::MenuContextToString(const ViewportMenuContext& context)
  {
    switch (context)
    {
		  case CreateMenu: return "Create Menu"; break;
		  case EntityProperties: return "Entity Properties"; break;
    }

    return nullptr;
  }

  void Editor::VpGui()
  {
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));

    ImGuiWindowFlags window_flags = ImGuiWindowFlags_NoScrollbar
      | ImGuiWindowFlags_NoScrollWithMouse
      | ImGuiWindowFlags_NoCollapse;

    ImGuiWindowClass window_class;
    window_class.DockNodeFlagsOverrideSet = ImGuiDockNodeFlags_AutoHideTabBar;

    ImGui::SetNextWindowClass(&window_class);
    ImGui::Begin("Viewport", nullptr, window_flags);

    m_ViewportHovered = ImGui::IsWindowHovered();
    m_ViewportFocused = ImGui::IsWindowFocused();
    if (ImGui::IsWindowFocused())
      m_SceneHierarchy.SetHierarchyMenuActive(false);
    else
      m_SceneHierarchy.SetHierarchyMenuActive(true);

    ViewportMenu();
    ViewportToolbar();

    ImVec2& viewportMinRegion = ImGui::GetWindowContentRegionMin();
    ImVec2& viewportMaxRegion = ImGui::GetWindowContentRegionMax();
    ImVec2& viewportOffset = ImGui::GetWindowPos();
    m_ViewportBounds[0] = { viewportMinRegion.x + viewportOffset.x, viewportMinRegion.y + viewportOffset.y };
    m_ViewportBounds[1] = { viewportMaxRegion.x + viewportOffset.x, viewportMaxRegion.y + viewportOffset.y };

    // Debug Info Overlay
    if (guiOverlay)
    {
      ImGuiWindowFlags window_flags = ImGuiWindowFlags_NoDecoration
        //      | ImGuiWindowFlags_NoDocking
        | ImGuiWindowFlags_AlwaysAutoResize
        | ImGuiWindowFlags_NoSavedSettings
        | ImGuiWindowFlags_NoFocusOnAppearing
        | ImGuiWindowFlags_NoNav;

      ImGui::SetNextWindowPos(
        { (viewportMinRegion.x + viewportOffset.x) + 8.0f,
        (viewportMinRegion.y + viewportOffset.y) + 8.0f }, ImGuiCond_Always);

      ImGui::SetNextWindowBgAlpha(0.0f); // Transparent background
      if (ImGui::Begin("top left overlay", nullptr, window_flags))
      {
        ImGui::Text("%.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
        ImGui::Text("Mouse Position (%d, %d)", mouseX, mouseY);
        std::string name = "None";
        if (m_HoveredEntity) name = m_HoveredEntity.GetComponent<TagComponent>().Tag;
        ImGui::Text("Hovered Entity: (%s) (%d)", name.c_str(), m_PixelData);
        ImGui::Text("Menu Context : %s", MenuContextToString(m_VpMenuContext));
      }
      ImGui::End();
    }

    ImVec2& viewportPanelSize = ImGui::GetContentRegionAvail();
    m_ViewportSize = { viewportPanelSize.x, viewportPanelSize.y };

    uint64_t viewportID = m_Framebuffer->GetColorAttachmentRendererID(m_RenderTarget);
    ImGui::Image(reinterpret_cast<void*>(viewportID), ImVec2(m_ViewportSize.x, m_ViewportSize.y), ImVec2(0, 1), ImVec2(1, 0));
    if (ImGui::BeginDragDropTarget())
    {
      if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("CONTENT_BROWSER_ITEM"))
      {
        if (m_SceneState == SceneState::Edit)
        {
          if (m_HoveredEntity)
          {
            m_SceneHierarchy.SetSelectedEntity(m_HoveredEntity);
            auto entity = m_SceneHierarchy.GetSelectedEntity();
            if (entity.HasComponent<SpriteRendererComponent>())
            {
              const wchar_t* path = (const wchar_t*)payload->Data;
              std::filesystem::path textureFile = std::filesystem::path(g_AssetPath) / path;
              auto& component = entity.GetComponent<SpriteRendererComponent>();
              if (textureFile.extension() == ".png" || textureFile.extension() == ".jpg")
              {
                component.Texture = Texture2D::Create(textureFile.string());
                component.TexturePath = textureFile.string();
              }
            }
          }
          else
          {
            const auto* path = static_cast<const wchar_t*>(payload->Data);
            auto scenePath = std::filesystem::path(g_AssetPath) / path;
            if (scenePath.extension() == ".org" || scenePath.extension() == ".origin")
            {
              OpenScene(scenePath);
            }
          }
        }
      }
      ImGui::EndDragDropTarget();
    }

    { // Gizmos
      m_SelectedEntity = m_SceneHierarchy.GetSelectedEntity();
      ImGuizmo::SetOrthographic(false); // by default is false
      ImGuizmo::SetDrawlist();

      if (m_SelectedEntity && m_GizmosType != -1)
      {
        ImGuizmo::SetRect(
          m_ViewportBounds[0].x, m_ViewportBounds[0].y,
          m_ViewportBounds[1].x - m_ViewportBounds[0].x,
          m_ViewportBounds[1].y - m_ViewportBounds[0].y
        );

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
          Math::DecomposeTransform(transform, translation, rotation, scale);

          tc.Translation = translation;
          glm::vec3 deltaRotation = rotation - tc.Rotation;
          tc.Rotation += deltaRotation;
          tc.Scale = scale;
        }

        if (ImGui::IsWindowFocused() && Input::IsKeyPressed(Key::Escape)) m_GizmosType = -1;
      }

      if (m_SelectedEntity != m_SceneHierarchy.GetSelectedEntity()) m_GizmosType = -1;
    }

    if (ImGuizmo::IsUsing() || !m_ViewportHovered)
      m_EditorCamera.EnableMovement(false);
    else
      m_EditorCamera.EnableMovement(true);

    ImGui::End();
    ImGui::PopStyleVar();
  }

  void Editor::OnScenePlay()
  {
    m_SceneState = SceneState::Play;

    m_ActiveScene = Scene::Copy(m_EditorScene);
    m_ActiveScene->OnRuntimeStart();

    m_SceneHierarchy.SetContext(m_ActiveScene);
  }

  void Editor::OnSceneStop()
  {
    m_SceneState = SceneState::Edit;

    m_ActiveScene->OnRuntimeStop();
    m_ActiveScene = m_EditorScene;

    m_SceneHierarchy.SetContext(m_ActiveScene);
  }

  void Editor::NewScene()
  {
		if (m_SceneState == SceneState::Play)
			OnSceneStop();

    m_HoveredEntity = {};
    m_SceneHierarchy.SetSelectedEntity({});
    m_SelectedEntity = m_SceneHierarchy.GetSelectedEntity();

    m_ActiveScene = std::make_shared<Scene>();
    m_ActiveScene->OnViewportResize((uint32_t)m_ViewportSize.x, (uint32_t)m_ViewportSize.y);
    m_EditorCamera.SetViewportSize(m_ViewportSize.x, m_ViewportSize.y);

    m_SceneHierarchy.SetContext(m_ActiveScene);
    m_EditorScene = m_ActiveScene;
  }

	void Editor::SaveScene()
	{
    if (!m_ScenePath.empty())
      SerializeScene(m_EditorScene, m_ScenePath);
    else SaveSceneAs();
	}

	void Editor::SaveSceneAs()
  {
    std::string filepath = FileDialogs::SaveFile("ORigin Scene (*.org,*.origin)\0*.org\0");

    if (!filepath.empty())
    {
      SerializeScene(m_EditorScene, m_ScenePath);
      m_ScenePath = filepath;
    }
  }

  void Editor::OpenScene(const std::filesystem::path& path)
  {
		if (m_SceneState == SceneState::Play)
			OnSceneStop();

    m_HoveredEntity = {};
    m_SceneHierarchy.SetSelectedEntity({});
    m_SelectedEntity = m_SceneHierarchy.GetSelectedEntity();

    std::shared_ptr<Scene> newScene = std::make_shared<Scene>();
    SceneSerializer serializer(newScene);
    if (serializer.Deserialize(path.string()))
    {
      m_EditorScene = newScene;
      m_EditorScene->OnViewportResize(static_cast<uint32_t>(m_ViewportSize.x), static_cast<uint32_t>(m_ViewportSize.y));
			m_EditorCamera.SetViewportSize(m_ViewportSize.x, m_ViewportSize.y);
      m_SceneHierarchy.SetContext(m_EditorScene);

      m_ActiveScene = m_EditorScene;
      m_ScenePath = path;
    }
  }

  void Editor::OpenScene()
  {
    if (m_SceneState == SceneState::Play)
      OnSceneStop();

    std::string filepath = FileDialogs::OpenFile("ORigin Scene (*.org,*.origin)\0*.org\0");
    if (!filepath.empty()) OpenScene(filepath);
  }

	void Editor::SerializeScene(std::shared_ptr<Scene>& scene, const std::filesystem::path& scenePath)
	{
    SceneSerializer serializer(scene);
    serializer.Serialize(scenePath);
	}

	void Editor::MenuBar()
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
        if (ImGui::MenuItem("New", "Ctrl + N")) NewScene();
        if (ImGui::MenuItem("Open", "Ctrl+O"))  OpenScene();
        if (ImGui::MenuItem("Save As", "Ctrl+Shift+S"))  SaveSceneAs();
        if (ImGui::MenuItem("Exit")) window.Close();
        ImGui::EndMenu();
      }

      if (ImGui::BeginMenu("Window"))
      {
        ImGui::MenuItem("Style Editor", nullptr, &guiMenuStyle);
        ImGui::MenuItem("Render Status", nullptr, &guiRenderStatus);
        ImGui::MenuItem("Debug Info", nullptr, &guiDebugInfo);
        ImGui::MenuItem("Debug Overlay", nullptr, &guiOverlay);
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

    if (guiRenderStatus)
    {
      ImGui::Begin("Render Status", &guiRenderStatus);

			ImGui::Text("Grid");
			ImGui::Text("Size"); ImGui::SameLine(0.0f, 1.5f); ImGui::DragInt("##grid_size", &m_GridSize);
			ImGui::Text("Color"); ImGui::SameLine(0.0f, 1.5f); ImGui::ColorEdit4("##grid_color", glm::value_ptr(m_GridColor));
			m_ActiveScene->SetGrid(m_GridSize, m_GridColor);

      ImGui::Separator();

      const auto stats = Renderer2D::GetStats();
      ImGui::Text("Draw Calls: %d", stats.DrawCalls);
      ImGui::Text("Quads %d", stats.QuadCount);
      ImGui::SameLine(); ImGui::Text("Circles %d", stats.CircleCount);
      ImGui::SameLine(); ImGui::Text("Lines %d", stats.LineCount);
      ImGui::Text("Vertices: %d", stats.GetTotalVertexCount());
      ImGui::Text("Indices: %d", stats.GetTotalIndexCount());
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
      ImGui::Text("Viewport Hovered (%d)", m_ViewportHovered);
      ImGui::Text("Hierarchy Menu Activity (%d)", m_SceneHierarchy.GetHierarchyMenuActive());

      ImGui::Separator();
      ImGui::Text("Editor Camera");
      ImGui::Text("Position (%.3f %.3f %.3f)", m_EditorCamera.GetPosition().x,
        m_EditorCamera.GetPosition().y, m_EditorCamera.GetPosition().z);

      ImGui::Text("Pitch (%.3f)", m_EditorCamera.GetPitch());
      ImGui::Text("YAW (%.3f)", m_EditorCamera.GetYaw());
      ImGui::Text("Hold Time (%.2f)", RMHoldTime);

      ImGui::Separator();

      ImGui::End();
    }

    if (guiImGuiDemoWindow)
    {
      ImGui::ShowDemoWindow(&guiImGuiDemoWindow);
    }
  }
  bool Editor::OnKeyPressed(KeyPressedEvent& e)
  {
    auto& app = Application::Get();
    bool control = Input::IsKeyPressed(Key::LeftControl) || Input::IsKeyPressed(Key::RightControl);
    bool shift = Input::IsKeyPressed(Key::LeftShift) || Input::IsKeyPressed(Key::RightShift);

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

      case Key::E:
      {
        if (!ImGuizmo::IsUsing() && !io.WantTextInput)
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
    }
    return true;
  }

  bool Editor::OnMouseButtonPressed(MouseButtonPressedEvent& e)
  {
    if (e.GetMouseButton() == Mouse::ButtonLeft && m_ViewportHovered)
    {
      if (Input::IsKeyPressed(Key::LeftControl)) {
        if (m_HoveredEntity == m_SelectedEntity && !ImGuizmo::IsOver())
        {
          m_GizmosType == -1 ? m_GizmosType = ImGuizmo::OPERATION::TRANSLATE : m_GizmosType == ImGuizmo::OPERATION::TRANSLATE ?
            m_GizmosType = ImGuizmo::OPERATION::ROTATE : m_GizmosType == ImGuizmo::OPERATION::ROTATE ? m_GizmosType = ImGuizmo::OPERATION::SCALE : m_GizmosType = -1;
        }
      }
    }
    return false;
  }

  void Editor::InputProccedure(Timestep time)
  {
    if (Input::IsMouseButtonPressed(Mouse::ButtonLeft))
    {
      if (m_ViewportHovered)
      {
        if (m_HoveredEntity && !ImGuizmo::IsOver())
        {
          m_SceneHierarchy.SetSelectedEntity(m_HoveredEntity);
        }

        else if (!m_HoveredEntity && !ImGuizmo::IsOver())
        {
          m_GizmosType = -1;
          m_SceneHierarchy.SetSelectedEntity({});
        }
      }
    }
    if (Input::IsMouseButtonPressed(Mouse::ButtonRight))
    {
      lastMouseX = lastMouseX;
      lastMouseY = lastMouseY;
      RMHoldTime += time.GetDeltaTime();

      // less than 1.5 sec
      /*if (RMHoldTime < 1900.0f && lastMouseX == mouseX && lastMouseY == mouseY) VpMenuContextActive = true;
      else if (RMHoldTime >= 1900.0f || lastMouseX != mouseX && lastMouseY != mouseY) VpMenuContextActive = false;*/

			if (lastMouseX == mouseX && lastMouseY == mouseY) VpMenuContextActive = true;
			else if (lastMouseX != mouseX && lastMouseY != mouseY) VpMenuContextActive = false;
    }
		else
		{
      lastMouseX = mouseX;
      lastMouseY = mouseY;
      RMHoldTime = 0.0f;
    }

    if (Input::IsMouseButtonPressed(Mouse::ButtonRight))
    {
      if (!m_HoveredEntity)
      {
        m_VpMenuContext = ViewportMenuContext::CreateMenu;
      }
      else
      {
        if (lastMouseX == mouseX && lastMouseY == mouseY)
        {
          m_SceneHierarchy.SetSelectedEntity(m_HoveredEntity);
          m_VpMenuContext = ViewportMenuContext::EntityProperties;
        }
      }
    }
  }

  void Editor::ViewportMenu()
  {
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(8, 8));

    if (VpMenuContextActive)
    {
      if (ImGui::BeginPopupContextWindow(nullptr, 1, false))
      {
        // Create Menu
        if (m_VpMenuContext == ViewportMenuContext::CreateMenu)
        {
          ImGui::Text("Add"); ImGui::Separator();

					if (ImGui::MenuItem("Empty")) m_SceneHierarchy.GetContext()->CreateEntity("Empty");
					if (ImGui::MenuItem("Camera")) m_SceneHierarchy.GetContext()->CreateCamera("Camera");
          ImGui::Separator();

          ImGui::Text("2D"); ImGui::Separator();
					if (ImGui::MenuItem("Sprite"))  m_SceneHierarchy.GetContext()->CreateSpriteEntity("Sprite");
					if (ImGui::MenuItem("Circle"))  m_SceneHierarchy.GetContext()->CreateCircle("Circle");
        }

        // Entity Properties
        if (m_VpMenuContext == ViewportMenuContext::EntityProperties)
        {
          // Entity Properties
          std::string name = "None";
          m_SelectedEntity ? name = m_SelectedEntity.GetComponent<TagComponent>().Tag : name;
          ImGui::Text("%s", name.c_str());
          ImGui::Separator();

          // destroy/remove entity
          bool entityDeleted = false;
          if (ImGui::MenuItem("Delete")) entityDeleted = true;

          if (entityDeleted)
          {
            Entity entity{ m_SelectedEntity, m_SceneHierarchy.GetContext().get() };
            m_SceneHierarchy.DestroyEntity(entity);

            m_HoveredEntity = {};
            m_SelectedEntity = m_SceneHierarchy.GetSelectedEntity();
          }

          if (ImGui::BeginMenu("Properties"))
          {
            ImGui::Text("Rename");
            if (m_SelectedEntity.HasComponent<TagComponent>())
            {
              auto& tag = m_SelectedEntity.GetComponent<TagComponent>().Tag;
              char buffer[64];
              memset(buffer, 0, sizeof(buffer));
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
              if (component.Texture)
              {
                ImGui::Text("Texture");
                ImGui::DragFloat("Tilling Factor", &component.TillingFactor, 0.1f, 0.0f, 10.0f);
                if (ImGui::Button("Delete", ImVec2(64.0f, 24.0f)))
                {
                  component.Texture->Delete();
                  component.Texture = {};
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

#if 0
    if (ImGui::BeginPopupContextWindow(0, 1, false))
    {
      if (m_PixelData == -1)
      {
        if (ImGui::BeginMenu("Create"))
        {
          if (ImGui::MenuItem("Empty")) m_SceneHierarchy.GetContext()->CreateEntity();
          if (ImGui::MenuItem("Camera")) m_SceneHierarchy.GetContext()->CreateCamera("Camera");

          if (ImGui::BeginMenu("2D"))
          {
            if (ImGui::MenuItem("2D Sprite"))  m_SceneHierarchy.GetContext()->CreateSpriteEntity();
            ImGui::EndMenu();
          }
          ImGui::EndMenu(); // ! Create Menu
        }
      }

      else if (m_HoveredEntity == m_SelectedEntity)
      {
        // Entity Properties
        std::string name = "None";
        m_SelectedEntity ? name = m_SelectedEntity.GetComponent<TagComponent>().Tag : name;
        glm::vec4& color = m_SelectedEntity.GetComponent<SpriteRendererComponent>().Color;
        ImGui::Text("%s", name.c_str());
        ImGui::Separator();

        // destroy/remove entity
        bool entityDeleted = false;
        if (ImGui::MenuItem("Delete")) entityDeleted = true;

        if (entityDeleted)
        {
          Entity entity{ m_SelectedEntity, m_SceneHierarchy.GetContext().get() };
          m_SceneHierarchy.DestroyEntity(entity);

          m_HoveredEntity = {};
          m_SelectedEntity = m_SceneHierarchy.GetSelectedEntity();
        }

        if (ImGui::BeginMenu("Properties"))
        {
          ImGui::Text("Rename");
          if (m_SelectedEntity.HasComponent<TagComponent>())
          {
            auto& tag = m_SelectedEntity.GetComponent<TagComponent>().Tag;
            char buffer[64];
            memset(buffer, 0, sizeof(buffer));
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
            ImGui::ColorEdit4("Color", glm::value_ptr(color));
            if (component.Texture)
            {
              ImGui::Text("Texture");
              ImGui::DragFloat("Tiling Factor", &component.TilingFactor, 0.1f, 0.0f, 10.0f);
              if (ImGui::Button("Delete", ImVec2(64.0f, 24.0f)))
              {
                component.Texture->Delete();
                component.Texture = {};
              }
            }
            ImGui::EndMenu();
          }
        }
      }
      ImGui::EndPopup(); // ! Create Popup
    }
#endif

    ImGui::PopStyleVar();
  }

  bool Editor::OnWindowResize(WindowResizeEvent& e)
  {
    return false;
  }

  bool Editor::OnMouseMovedEvent(MouseMovedEvent& e)
  {
    return false;
  }

  bool Editor::OnMouseButtonEvent(MouseButtonEvent& e)
  {
    return false;
  }
}