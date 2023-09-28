// Copyright (c) 2022 Evangelion Manuhutu | ORigin Engine

#include "Editor.h"

#include "Origin/EntryPoint.h"
#include "Origin/Utils/PlatformUtils.h"
#include "Origin/Scene/SceneSerializer.h"
#include "Origin/Scripting/ScriptEngine.h"
#include "Origin/Renderer/Font.h"

#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <imgui_internal.h>
#include <filesystem>

namespace origin {

  Editor* Editor::s_Instance = nullptr;

  Editor::Editor() : Layer("Editor")
  {
    s_Instance = this;
  }

  Editor::~Editor()
  {
    ScriptEngine::Shutdown();
  }

  void Editor::OnAttach()
  {
    m_PlayButton = Texture2D::Create("Resources/UITextures/playbutton.png");
    m_SimulateButton = Texture2D::Create("Resources/UITextures/simulatebutton.png");
    m_StopButton = Texture2D::Create("Resources/UITextures/stopbutton.png");
    m_PauseButton = Texture2D::Create("Resources/UITextures/pausebutton.png");
    m_SteppingButton = Texture2D::Create("Resources/UITextures/steppingframebutton.png");

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
    m_EditorCamera.SetPosition(glm::vec3(0.0f, 0.0f, 10.0f));

    // Load Skybox
    skybox = Skybox::Create("Resources/Skybox");

    m_ActiveScene = std::make_shared<Scene>();
    const auto commandLineArgs = Application::Get().GetSpecification().CommandLineArgs;
    if (commandLineArgs.Count > 1)
    {
        m_ProjectPath = commandLineArgs[1];
        OpenProject(m_ProjectPath);
    }
    else
    {
        OpenProject();
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
    m_ActiveScene->OnShadowRender();

    m_Time += time.Seconds();
    const ImGuiIO& io = ImGui::GetIO();

    const bool enableCamera =
        io.WantTextInput == false
        || !ImGuizmo::IsUsing()
        || Application::Get().GetGuiLayer()->GetActiveWidgetID() == 0;

    m_EditorCamera.EnableMovement(enableCamera);

    Renderer2D::ResetStats();
    Renderer3D::ResetStats();
    InputProcedure(time);

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

    if (enableSkybox)
        skybox->Draw(m_EditorCamera);

    switch (m_SceneState)
    {
    case SceneState::Play:
        m_GizmosType = -1;
        m_ActiveScene->OnViewportResize(static_cast<uint32_t>(m_SceneViewportSize.x),
                                        static_cast<uint32_t>(m_SceneViewportSize.y));
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

    OnOverlayRenderer();
    m_Framebuffer->Unbind();
  }

  void Editor::OnDuplicateEntity()
  {
      if (m_SceneState != SceneState::Edit)
          return;

      if (Entity selectedEntity = m_SceneHierarchy.GetSelectedEntity())
      {
          Entity entity = m_EditorScene->DuplicateEntity(selectedEntity);
          m_SceneHierarchy.SetSelectedEntity(entity);
      }
  }

  void Editor::OnGuiRender()
  {
      m_Dockspace.Begin();
      MenuBar();
      SceneViewport();
      m_SceneHierarchy.OnImGuiRender();
      m_ContentBrowser->OnImGuiRender();
      GUIRender();
      m_Dockspace.End();
  }

  void Editor::OverlayBeginScene()
  {
      if (m_SceneState == SceneState::Play)
      {
          if (Entity camera = m_ActiveScene->GetPrimaryCameraEntity())
          {
              const glm::mat4& transform = camera.GetComponent<TransformComponent>().GetTransform();
              Renderer::BeginScene(camera.GetComponent<CameraComponent>().Camera, transform);
          }
      }
      else Renderer::BeginScene(m_EditorCamera);
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
              const auto& view = m_ActiveScene->GetAllEntitiesWith<TransformComponent, CircleCollider2DComponent>();
              for (auto entity : view)
              {
                  const auto& [tc, cc2d] = view.get<TransformComponent, CircleCollider2DComponent>(entity);

                  glm::vec3 translation = tc.Translation + glm::vec3(cc2d.Offset, -projectionRender.z);
                  glm::vec3 scale = tc.Scale * glm::vec3(cc2d.Radius * 2.0f);

                  glm::mat4 transform = glm::translate(glm::mat4(1.0f), translation)
                      * glm::scale(glm::mat4(1.0f), scale);

                  Renderer2D::DrawCircle(transform, glm::vec4(0, 1, 0, 1), 0.05f);
              }
          }

          // Quad Collider Visualizer
          {
              const auto& view = m_ActiveScene->GetAllEntitiesWith<TransformComponent, BoxCollider2DComponent>();
              for (auto entity : view)
              {
                  const auto& [tc, bc2d] = view.get<TransformComponent, BoxCollider2DComponent>(entity);

                  glm::vec3 scale = tc.Scale * glm::vec3(bc2d.Size * 2.0f, 1.0f);

                  glm::mat4 transform = glm::translate(glm::mat4(1.0f), tc.Translation)
                      * glm::rotate(glm::mat4(1.0f), tc.Rotation.z, glm::vec3(0.0f, 0.0f, 1.0f))
                      * glm::translate(glm::mat4(1.0f), glm::vec3(bc2d.Offset, projectionRender.z))
                      * glm::scale(glm::mat4(1.0f), scale);

                  Renderer2D::DrawRect(transform, glm::vec4(0.0f, 1.0f, 0.0f, 1.0f));
              }
          }
      }
      RenderCommand::SetLineWidth(2.3f);
      Renderer2D::EndScene();

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

  void Editor::OnScenePlay()
  {
      if (m_SceneState == SceneState::Simulate)
          OnSceneStop();

      m_SceneState = SceneState::Play;

      m_ActiveScene = Scene::Copy(m_EditorScene);
      m_ActiveScene->OnRuntimeStart();

      m_SceneHierarchy.SetContext(m_ActiveScene);
  }

  void Editor::OnScenePause()
  {
      if (m_SceneState == SceneState::Edit)
          return;

      m_ActiveScene->SetPaused(true);
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
    if (m_SceneState == SceneState::Play)
        m_ActiveScene->OnRuntimeStop();
    else if (m_SceneState == SceneState::Simulate)
        m_ActiveScene->OnSimulationStop();

    m_SceneHierarchy.SetContext(m_EditorScene);
    m_ActiveScene = m_EditorScene;

    m_SceneState = SceneState::Edit;
  }

  void Editor::NewProject()
  {
      Project::New();
  }

  void Editor::OpenProject(const std::filesystem::path& path)
  {
    if (Project::Load(path))
    {
      const auto startScenePath = Project::GetAssetFileSystemPath(Project::GetActive()->GetConfig().StartScene);

      OpenScene(startScenePath);
      ScriptEngine::Init();

      m_ContentBrowser = std::make_unique<ContentBrowserPanel>();
    }
  }

  void Editor::OpenProject()
  {
    if (Project::Open())
    {
      const auto startScenePath = Project::GetAssetFileSystemPath(Project::GetActive()->GetConfig().StartScene);

      ScriptEngine::Init();

      OpenScene(startScenePath);
      m_ProjectPath = Project::GetProjectDirectory();

      m_ContentBrowser = std::make_unique<ContentBrowserPanel>();
    }
  }

  void Editor::SaveProject()
  {
    Project::SaveActive(m_ProjectPath);
  }

  void Editor::NewScene()
  {
    if (m_SceneState == SceneState::Play || m_SceneState == SceneState::Simulate)
      OnSceneStop();

    m_EditorScene = std::make_shared<Scene>();

    m_SceneHierarchy.SetContext(m_EditorScene, true);

    m_ActiveScene = m_EditorScene;
    m_ScenePath = std::filesystem::path();
  }

  void Editor::SaveScene()
  {
    if (m_SceneState == SceneState::Play || m_SceneState == SceneState::Simulate)
        OnSceneStop();

    if (!m_ScenePath.empty())
      SerializeScene(m_ActiveScene, m_ScenePath);
    else
        SaveSceneAs();
  }

  void Editor::SaveSceneAs()
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

  void Editor::OpenScene(const std::filesystem::path& path)
  {
    if (m_SceneState == SceneState::Play || m_SceneState == SceneState::Simulate)
      OnSceneStop();

    auto newScene = std::make_shared<Scene>();

    SceneSerializer serializer(newScene);
    if (serializer.Deserialize(path.string()))
    {
      m_HoveredEntity = {};
      m_EditorScene = newScene;
      m_SceneHierarchy.SetContext(m_EditorScene, true);

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
