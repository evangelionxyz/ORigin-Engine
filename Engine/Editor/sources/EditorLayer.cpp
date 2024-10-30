// Copyright (c) Evangelion Manuhutu | ORigin Engine

#include "EditorLayer.h"
#include "Origin/EntryPoint.h"
#include "Gizmos/Gizmos.h"
#include "Origin/Utils/PlatformUtils.h"
#include "Origin/Scripting/ScriptEngine.h"
#include "Origin/Asset/AssetManager.h"
#include "Origin/Asset/AssetImporter.h"
#include "Panels/AnimationTimeline.h"
#include "Origin/GUI/UI.h"

#include "Serializer/EditorSerializer.h"

#include <filesystem>

#include <glad/glad.h>

namespace origin
{
    static EditorLayer *s_Instance = nullptr;
    EditorLayer::EditorLayer() : Layer("EditorLayer") { s_Instance = this; }

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
              FramebufferTextureFormat::DEPTH24STENCIL8
        };

        fbSpec.Width = 1280;
        fbSpec.Height = 720;
        m_Framebuffer = Framebuffer::Create(fbSpec);

        if (const auto filepath = std::filesystem::current_path() / "Editor.cfg";
            !EditorSerializer::Deserialize(this, filepath))
        {
            m_EditorCamera.InitPerspective(45.0f, 1.776f, 0.1f, 1000.0f);
            m_EditorCamera.InitOrthographic(10.0f, 0.1f, 100.0f);
        }

        m_ActiveScene = std::make_shared<Scene>();

        if (const auto &cmdline_args = Application::Instance().GetSpecification().CommandLineArgs;
            cmdline_args.Count > 1)
        {
            m_ProjectDirectoryPath = cmdline_args[1];
            OpenProject(m_ProjectDirectoryPath);
        }

        m_SpriteSheetEditor = std::make_unique<SpriteSheetEditor>();
        m_Gizmos = std::make_unique<Gizmos>();

        if (!m_UIEditor)
        {
            m_UIEditor = std::make_unique<UIEditor>(m_ActiveScene.get());
        }

        InitGrid();
    }

    void EditorLayer::OnDetach() 
    {
        std::filesystem::path filepath = std::filesystem::current_path() / "Editor.cfg";
        EditorSerializer::Serialize(this, filepath);
        ScriptEngine::Shutdown(); 
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

    void EditorLayer::OnUpdate(const Timestep ts)
    {
        OGN_PROFILER_FUNCTION();

        // calculate mouse
        auto [mx, my] = ImGui::GetMousePos();
        m_ViewportMousePos = { mx, my };
        m_ViewportMousePos -= m_SceneViewportBounds[0];
        m_ViewportMousePos.y = m_SceneViewportSize.y - m_ViewportMousePos.y;
        m_ViewportMousePos = glm::clamp(m_ViewportMousePos, { 0.0f, 0.0f }, m_SceneViewportSize - glm::vec2(1.0f, 1.0f));

        ProfilerTimer timer("EditorLayer::OnUpdate", [&](const ProfilerResult result)
        {
            m_ProfilerResults.push_back(result);
        });

        switch (m_SceneState)
        {
        case SceneState::Edit:
        case SceneState::Simulate:
        {
            if (const auto &fb_spec = m_Framebuffer->GetSpecification(); m_SceneViewportSize.x != fb_spec.Width || m_SceneViewportSize.y != fb_spec.Height)
            {
                if (m_SceneViewportSize.x > 0.0f && m_SceneViewportSize.y > 0.0f)
                {
                    m_Framebuffer->Resize(static_cast<u32>(m_SceneViewportSize.x), static_cast<u32>(m_SceneViewportSize.y));
                    m_EditorCamera.SetViewportSize(m_SceneViewportSize.x, m_SceneViewportSize.y);
                    m_ActiveScene->OnViewportResize(static_cast<u32>(m_SceneViewportSize.x), static_cast<u32>(m_SceneViewportSize.y));
                }

            }
            break;
        }
        case SceneState::Play:
            const u32 width = m_ActiveScene->GetWidth();
            const u32 height = m_ActiveScene->GetHeight();
            if (m_SceneViewportSize.x != width && m_SceneViewportSize.y != height)
            {
                if (m_SceneViewportSize.x > 0.0f && m_SceneViewportSize.y > 0.0f)
                {
                    m_ActiveScene->OnViewportResize(static_cast<u32>(m_SceneViewportSize.x),
                        static_cast<u32>(m_SceneViewportSize.y));
                }
                break;
            }
        }

        SystemUpdate(ts);

        m_ActiveScene->PreRender(m_EditorCamera, ts);

        m_Framebuffer->Bind();
        RenderCommand::ClearColor(m_ClearColor);
        RenderCommand::Clear();
        Render(ts);
        m_ActiveScene->GetUIRenderer()->Render();
        m_Framebuffer->Unbind();
        m_ActiveScene->PostRender(m_EditorCamera, ts);

        InputProcedure(ts);
    }

    EditorLayer &EditorLayer::Get()
    {
        return *s_Instance;
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
        {
            if (m_ActiveScene->IsFocusing())
            {
                Input::Get().SetMouseToCenter();
            }

            m_Gizmos->SetType(GizmoType::NONE);
            m_ActiveScene->OnUpdateRuntime(ts);
            if (Entity cam = m_ActiveScene->GetPrimaryCameraEntity())
            {
                CameraComponent cc = cam.GetComponent<CameraComponent>();
                if(m_VisualizeBoundingBox) m_Gizmos->DrawBoundingBox(cc.Camera, m_ActiveScene.get());
                if(m_VisualizeCollider) m_Gizmos->DrawCollider(cc.Camera, m_ActiveScene.get());
            }
            break;
        }

        case SceneState::Edit:
        {
            // update camera
            m_EditorCamera.SetAllowedMove(IsViewportFocused && IsViewportHovered && !ImGui::GetIO().WantTextInput);
            // m_EditorCamera.OnUpdate(ts, m_SceneViewportBounds[0], m_SceneViewportBounds[1]);
            m_EditorCamera.OnUpdate(ts);
                
            // draw gizmo
            m_Gizmos->DrawFrustum(m_EditorCamera, m_ActiveScene.get());
            if(m_VisualizeBoundingBox) m_Gizmos->DrawBoundingBox(m_EditorCamera, m_ActiveScene.get());
            if (m_VisualizeCollider) m_Gizmos->DrawCollider(m_EditorCamera, m_ActiveScene.get());
            if (m_Draw3DGrid) m_Gizmos->Draw3DGrid(m_EditorCamera, true, false, m_3DGridSize);
            if (m_Draw2DGrid) m_Gizmos->Draw2DGrid(m_EditorCamera);
            ShowGrid();

            // update scene
            m_ActiveScene->OnUpdateEditor(m_EditorCamera, ts, m_SceneHierarchy.GetSelectedEntity());
            m_Gizmos->DrawIcons(m_EditorCamera, m_ActiveScene.get());
            break;
        }

        case SceneState::Simulate:
        {
            // update camera
            m_EditorCamera.SetAllowedMove(IsViewportFocused && IsViewportHovered && !ImGui::GetIO().WantTextInput);
            //m_EditorCamera.OnUpdate(ts, m_SceneViewportBounds[0], m_SceneViewportBounds[1]);
            m_EditorCamera.OnUpdate(ts);


            // draw gizmo
            m_Gizmos->DrawFrustum(m_EditorCamera, m_ActiveScene.get());
            if (m_VisualizeBoundingBox) m_Gizmos->DrawBoundingBox(m_EditorCamera, m_ActiveScene.get());
            if (m_VisualizeCollider)m_Gizmos->DrawCollider(m_EditorCamera, m_ActiveScene.get());
            if (m_Draw3DGrid) m_Gizmos->Draw3DGrid(m_EditorCamera, true, false, m_3DGridSize);
            if (m_Draw2DGrid) m_Gizmos->Draw2DGrid(m_EditorCamera);
            ShowGrid();

            // update scene
            m_ActiveScene->OnUpdateSimulation(m_EditorCamera, ts, m_SceneHierarchy.GetSelectedEntity());
            m_Gizmos->DrawIcons(m_EditorCamera, m_ActiveScene.get());

            break;
        }
        }
    }

    void EditorLayer::SystemUpdate(const Timestep ts)
    {
        m_Time += Timestep::Seconds();

        Renderer::GetStatistics().Reset();
        if (m_SceneState != SceneState::Play)
        {
            m_EditorCamera.UpdateAudioListener(ts);
        }

        m_SpriteSheetEditor->OnUpdate(ts);
        m_UIEditor->OnUpdate(ts);
    }

    void EditorLayer::OnDuplicateEntity()
    {
        if (m_SceneState != SceneState::Edit)
            return;

        if (const Entity selectedEntity = m_SceneHierarchy.GetSelectedEntity())
        {
            const Entity entity = m_ActiveScene->DuplicateEntity(selectedEntity);
            m_SceneHierarchy.SetSelectedEntity(entity);
        }
    }

    void EditorLayer::OnDestroyEntity()
    {
        if (m_SceneState != SceneState::Edit)
            return;

        if (Entity selectedEntity = m_SceneHierarchy.GetSelectedEntity())
        {
            m_ActiveScene->DestroyEntity(selectedEntity);
            //CommandManager::Instance().ExecuteCommand(std::make_unique<DestoryEntityCommand>(m_ActiveScene, selectedEntity));
        }
    }

    void EditorLayer::OnGuiRender()
    {
        m_Dockspace.Begin();
        SceneViewport();
        m_UIEditor->OnImGuiRender();
        m_SpriteSheetEditor->OnImGuiRender();
        m_MaterialEditor.OnImGuiRender();
        m_SceneHierarchy.OnImGuiRender();
        ConsoleWindow();
        MenuBar();
        SceneViewportToolbar();
        GUIRender();
        if (m_ContentBrowser) m_ContentBrowser->OnImGuiRender();
        m_Dockspace.End();
    }

    void EditorLayer::OnScenePlay()
    {
        OGN_PROFILER_SCENE();

        if (m_SceneState == SceneState::Simulate)
        {
            OnSceneStop();
        }

        m_SceneState = SceneState::Play;
        m_ActiveScene = Scene::Copy(m_EditorScene);
        m_ActiveScene->OnRuntimeStart();
        m_SceneHierarchy.SetActiveScene(m_ActiveScene);
        //m_ActiveScene->LockMouse();
    }

    void EditorLayer::OnScenePause() const
    {
        OGN_PROFILER_SCENE();

        if (m_SceneState == SceneState::Edit)
        {
            return;
        }

        m_ActiveScene->SetPaused(true);
        //m_ActiveScene->UnlockMouse();
    }

    void EditorLayer::OnSceneSimulate()
    {
        OGN_PROFILER_RENDERING();

        if (m_SceneState == SceneState::Play)
        {
            OnSceneStop();
        }

        m_SceneState = SceneState::Simulate;

        m_ActiveScene = Scene::Copy(m_EditorScene);
        m_ActiveScene->OnRuntimeStart();
        m_SceneHierarchy.SetActiveScene(m_ActiveScene);
    }

    void EditorLayer::OnSceneStop()
    {
        OGN_PROFILER_SCENE();

        ScriptEngine::ClearSceneContext();

        if (m_SceneState == SceneState::Play || m_SceneState == SceneState::Simulate)
        {
            m_ActiveScene->OnRuntimeStop();
        }

        m_SceneHierarchy.SetActiveScene(m_EditorScene);
        m_ActiveScene = m_EditorScene;
        m_SceneState = SceneState::Edit;
        //m_ActiveScene->UnlockMouse();
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

        OGN_CORE_ERROR("Failed to open project");
        PUSH_CONSOLE_ERROR("Failed to open project");
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
        Project::SetActiveScene(m_ActiveScene);
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
            SerializeScene(m_ActiveScene, filepath);
            m_ScenePath = filepath;
        }
    }

    void EditorLayer::OpenScene(const AssetHandle handle)
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

        const AssetMetadata &metadata = Project::GetActive()->GetEditorAssetManager()->GetMetadata(handle);
        Ref<Scene> read_only_scene = AssetManager::GetAsset<Scene>(handle);

        if (!read_only_scene)
        {
            OGN_CORE_ERROR("[EditorLayer] Invalid scene!");
            PUSH_CONSOLE_ERROR("Invalid scene!");
            return;
        }

        m_EditorScene = Scene::Copy(read_only_scene);
        m_HoveredEntity = {};

        m_SceneHierarchy.SetActiveScene(m_EditorScene, true);
        m_ActiveScene = m_EditorScene;
        m_ScenePath = metadata.Filepath;
        std::string name = metadata.Filepath.stem().string();
        m_ActiveScene->SetName(name);

        Project::SetActiveScene(m_ActiveScene);

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

        const AssetHandle handle = SceneImporter::OpenScene(filepath);
        if (handle == 0 || filepath.empty())
            return;

        Ref<Scene> read_only_scene = AssetManager::GetAsset<Scene>(handle);
        std::string name = filepath.stem().string();
        read_only_scene->SetName(name);

        if (!read_only_scene)
        {
            OGN_CORE_ERROR("[EditorLayer] Invalid scene!");
            PUSH_CONSOLE_ERROR("Invalid scene!");
            return;
        }

        m_HoveredEntity = {};
        m_EditorScene = Scene::Copy(read_only_scene);
        m_SceneHierarchy.SetActiveScene(m_EditorScene, true);
        m_ActiveScene = m_EditorScene;

        Project::SetActiveScene(m_ActiveScene);

        m_ScenePath = filepath;

        if (!m_UIEditor)
            m_UIEditor = std::make_unique<UIEditor>(m_ActiveScene.get());

        m_UIEditor->SetContext(m_ActiveScene.get());
    }

    void EditorLayer::SerializeScene(const Ref<Scene> &scene, const std::filesystem::path &filepath)
    {
        scene->SetName(filepath.stem().string());
        SceneImporter::SaveScene(scene, filepath);
    }

    void EditorLayer::MenuBar()
    {
        auto &application = Application::Instance();

        ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(8.0f, 8.0f));

        if (ImGui::BeginMenuBar())
        {
            ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(8.0f, 3.0f));

            if (ImGui::BeginMenu("File"))
            {
                if (ImGui::MenuItem("New Project")) NewProject();
                if (ImGui::MenuItem("Open Project")) OpenProject();

                if (ImGui::MenuItem("Save Project", "", nullptr, static_cast<bool>(Project::GetActive())))
                {
                    SaveScene();
                    Project::SaveActive();
                }

                ImGui::Separator();
                if (ImGui::MenuItem("New Scene", "Ctrl+N", nullptr, static_cast<bool>(Project::GetActive()))) NewScene();
                if (ImGui::MenuItem("Open Scene", "Ctrl+O", nullptr, static_cast<bool>(Project::GetActive()))) OpenScene();
                if (ImGui::MenuItem("Save Scene", "Ctrl+S", nullptr, static_cast<bool>(Project::GetActive()))) SaveScene();
                if (ImGui::MenuItem("Save Scene As", "Ctrl+Shift+S", nullptr, static_cast<bool>(Project::GetActive()))) SaveSceneAs();
                if (ImGui::MenuItem("Exit", "Alt+F4")) application.Close();

                ImGui::EndMenu();
            }

            if (ImGui::BeginMenu("Script"))
            {
                if (ImGui::MenuItem("Reload"))
                {
                    if (m_SceneState == SceneState::Play || m_SceneState == SceneState::Simulate)
                    {
                        OnSceneStop();
                    }
                    ScriptEngine::ReloadAssembly();
                }
                ImGui::EndMenu();
            }

            if (ImGui::BeginMenu("Window"))
            {
                ImGui::MenuItem("Render Settings", nullptr, &GuiRenderSettingsWindow);
                ImGui::MenuItem("Console", nullptr, &GuiConsoleWindow);
                ImGui::MenuItem("Style Editor", nullptr, &GuiMenuStyleWindow);
                ImGui::MenuItem("Demo Window", nullptr, &GuiImGuiDemoWindow);
                ImGui::EndMenu();
            }

            if (ImGui::BeginMenu("View"))
            {
                if (ImGui::MenuItem("Full Screen", "F11", &GuiMenuFullscreen)) application.GetWindow().ToggleFullScreen();
                if (ImGui::MenuItem("VSync", nullptr, &GuiVSync)) application.GetWindow().SetVSync(GuiVSync);
                ImGui::EndMenu();
            }

            if (ImGui::BeginMenu("Tools"))
            {
                ImGui::MenuItem("Preferences", nullptr, &GuiPreferencesWindow);

                ImGui::EndMenu();
            }

            ImGui::EndMenuBar();
            ImGui::PopStyleVar();
        }
        ImGui::PopStyleVar();
    }

    void EditorLayer::SceneViewport()
    {
        ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));
        ImGuiWindowFlags window_flags = ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse | ImGuiWindowFlags_NoCollapse;

        ImGui::Begin("Scene", nullptr, window_flags);

        IsViewportHovered = ImGui::IsWindowHovered();
        IsViewportFocused = ImGui::IsWindowFocused();

        const ImVec2 &viewportMinRegion = ImGui::GetWindowContentRegionMin();
        const ImVec2 &viewportMaxRegion = ImGui::GetWindowContentRegionMax();
        const ImVec2 &viewportOffset = ImGui::GetWindowPos();
        m_SceneViewportBounds[0] = { viewportMinRegion.x + viewportOffset.x, viewportMinRegion.y + viewportOffset.y };
        m_SceneViewportBounds[1] = { viewportMaxRegion.x + viewportOffset.x, viewportMaxRegion.y + viewportOffset.y };

        ImVec2 viewportPanelSize = ImGui::GetContentRegionAvail();
        m_SceneViewportSize = { viewportPanelSize.x, viewportPanelSize.y };

        if (m_SceneState == SceneState::Play)
        {
            for (auto cam_view = m_ActiveScene->m_Registry.view<CameraComponent>(); auto &e : cam_view)
            {
                if (auto &cc = cam_view.get<CameraComponent>(e); cc.Primary)
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

        ImTextureID viewportID = reinterpret_cast<void*>((uintptr_t)m_Framebuffer->GetColorAttachmentRendererID(m_RenderTarget));
        ImGui::Image(viewportID, ImVec2(m_GameViewportSize.x, m_GameViewportSize.y), ImVec2(0, 1), ImVec2(1, 0));
        if (ImGui::BeginDragDropTarget())
        {
            if (const ImGuiPayload *payload = ImGui::AcceptDragDropPayload("CONTENT_BROWSER_ITEM"))
            {
                if (m_SceneState == SceneState::Edit)
                {
                    AssetHandle handle = *static_cast<AssetHandle*>(payload->Data);
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

        // Gizmos
        ImGuizmo::SetDrawlist();

        glm::vec2 windowMin = { m_SceneViewportBounds[0].x, m_SceneViewportBounds[0].y };
        glm::vec2 windowMax = { m_SceneViewportBounds[1].x - m_SceneViewportBounds[0].x, m_SceneViewportBounds[1].y - m_SceneViewportBounds[0].y };

        ImGuizmo::SetRect(windowMin.x, windowMin.y, windowMax.x, windowMax.y);
        ImGuizmo::SetOrthographic(m_EditorCamera.GetProjectionType() == ProjectionType::Orthographic);

        const glm::mat4 &cameraProjection = m_EditorCamera.GetProjectionMatrix();
        const glm::mat4 &cameraView = m_EditorCamera.GetViewMatrix();

        Entity entity = m_SceneHierarchy.GetSelectedEntity();
        if (entity && m_Gizmos->GetType() != GizmoType::NONE)
        {
            auto &tc = entity.GetComponent<TransformComponent>();
            glm::mat4 transform = tc.GetTransform();
            auto &idc = entity.GetComponent<IDComponent>();

            float snapValues[] = { snapValue, snapValue, snapValue };
            float bounds[] = { -0.5f, -0.5f, -0.5f, 0.5f, 0.5f, 0.5f };
            bool boundSizing = m_Gizmos->GetType() == GizmoType::BOUNDARY
                && !entity.HasComponent<CameraComponent>()
                && !entity.HasComponent<LightComponent>()
                && !entity.HasComponent<AudioComponent>();

            bool snap = Input::Get().IsKeyPressed(KeyCode(Key::LeftShift));
            ImGuizmo::Manipulate(glm::value_ptr(cameraView), glm::value_ptr(cameraProjection),
                m_ImGuizmoOperation, static_cast<ImGuizmo::MODE>(m_GizmosMode), glm::value_ptr(transform), nullptr,
                snap ? snapValues : nullptr, boundSizing ? bounds : nullptr, snap ? snapValues : nullptr);
            
            static bool changed = false;
            if (ImGuizmo::IsUsing())
            {
                if (!changed)
                {
                    auto &temp = ComponentsCommand<TransformComponent>::GetTempComponent();
                    temp = tc;
                }

                glm::vec3 translation, rotation, scale;
                Math::DecomposeTransformEuler(transform, translation, rotation, scale);

                if (entity.HasParent())
                {
                    if (auto parent = m_ActiveScene->GetEntityWithUUID(idc.Parent))
                    {
                        auto &ptc = parent.GetComponent<TransformComponent>();
                        glm::vec4 localTranslation = glm::inverse(ptc.GetTransform()) * glm::vec4(translation, 1.0f);

                        // Apply parent's scale to local translation
                        localTranslation.x *= ptc.WorldScale.x;
                        localTranslation.y *= ptc.WorldScale.y;
                        localTranslation.z *= ptc.WorldScale.z;

                        // Convert back to world space
                        tc.Translation = glm::vec3(ptc.GetTransform() * localTranslation);
                        tc.Translation = glm::vec3(localTranslation);
                        tc.Rotation = glm::inverse(ptc.WorldRotation) * glm::quat(rotation);
                        tc.Scale = scale;
                    }
                }
                else
                {
                    tc.Translation = translation;
                    glm::vec3 localEuler = glm::eulerAngles(tc.Rotation);
                    localEuler += rotation - localEuler;
                    tc.Rotation = glm::quat(localEuler);

                    tc.Scale = scale;
                }
                changed = true;
            }

            if (changed && !ImGuizmo::IsUsing())
            {
                if (tc != ComponentsCommand<TransformComponent>::GetTempComponent())
                {
                    CommandManager::Instance().ExecuteCommand(
                    std::make_unique<ComponentsCommand<TransformComponent>>(tc, 
                        ComponentsCommand<TransformComponent>::GetTempComponent()));
                    changed = false;
                }
            }
        }

        if (ImGui::IsWindowFocused() && ImGui::IsWindowHovered() && ImGui::IsMouseClicked(ImGuiMouseButton_Left) && m_SceneState == SceneState::Play)
        {
            m_ActiveScene->LockMouse();
        }

        ImGui::End();
        ImGui::PopStyleVar();
    }

    void EditorLayer::SceneViewportToolbar()
    {
        ImGuiWindowFlags window_flags = ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse;

        if (ImGui::Begin("Toolbar", nullptr, window_flags))
        {
            const ImVec2 bt_size = {16.0f, 16.0f};

            // canvas position from the top left relative to monitor
            const ImVec2 &canvas_top_left = ImGui::GetCursorScreenPos();
            const ImVec2 &canvas_size = ImVec2(ImGui::GetContentRegionAvail().x, bt_size.y);

            ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0, 0, 0, 0));
            ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.3f, 0.3f, 0.3f, 0.3f));
            ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0, 0, 0, 0));

            // margin left: 16px
            // margin top: 4px
            ImGui::SetCursorPos({8.0f, 0.0f});
            // Play Button
            Ref<Texture2D> icon = (m_SceneState == SceneState::Edit || m_SceneState == SceneState::Simulate) ? m_UITextures.at("play") : m_UITextures.at("stop");
            if (ImGui::ImageButton("play_button", reinterpret_cast<void *>(static_cast<uintptr_t>(icon->GetTextureID())), bt_size))
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
            if (ImGui::ImageButton("simulate_button", reinterpret_cast<void *>(static_cast<uintptr_t>(icon->GetTextureID())), bt_size))
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
            // Pause Button
            if (m_SceneState != SceneState::Edit)
            {
                ImGui::SameLine();
                bool isPaused = m_ActiveScene->IsPaused();
                icon = m_UITextures.at("pause");
                if (ImGui::ImageButton("pause_button", (void *)(uintptr_t)icon->GetTextureID(), bt_size))
                {
                    m_ActiveScene->SetPaused(!isPaused);
                }

                if (isPaused)
                {
                    icon = m_UITextures.at("stepping");
                    ImGui::SameLine();
                    if (ImGui::ImageButton("stepping_button", (void *)(uintptr_t)icon->GetTextureID(), bt_size))
                    {
                        m_ActiveScene->Step(1);
                    }
                }
            }

            ImGui::SameLine();

            // Projection mode
            const auto &mode = m_EditorCamera.GetProjectionType();
            icon = mode == ProjectionType::Orthographic ? m_UITextures.at("camera_2d_projection") : m_UITextures.at("camera_3d_projection");
            if (ImGui::ImageButton("projection_button", (void *)(uintptr_t)icon->GetTextureID(), bt_size, ImVec2(0, 1), ImVec2(1, 0)))
            {
                if (mode == ProjectionType::Perspective)
                    m_EditorCamera.SetProjectionType(ProjectionType::Orthographic);
                else if (mode == ProjectionType::Orthographic)
                    m_EditorCamera.SetProjectionType(ProjectionType::Perspective);
            }
            ImGui::PopStyleColor(3);

            // margin left: 200px
            // font size: 16px
            // margin top: 4px (16.0f / 4.0f)
            ImGui::SetCursorPos({canvas_size.x - 200.0f, 4.0f});
            ImGui::Text("%.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
            ImGui::End(); // !Toolbar
        }
    }

    void EditorLayer::GUIRender()
    {
        Entity entity = m_SceneHierarchy.GetSelectedEntity();
        if (entity)
        {
            if (entity.HasComponent<SpriteAnimationComponent>())
            {
                auto &ac = entity.GetComponent<SpriteAnimationComponent>();
                AnimationTimeline::DrawSpriteAnimTimeline(ac);
            }
        }

        if (GuiPreferencesWindow)
        {
            static int selected_option = 0;

            ImGui::Begin("Preferences", &GuiPreferencesWindow);

            // Define a child window for the left menu
            ImGui::BeginChild("LeftMenu", {200.0f, 0.0f}, ImGuiChildFlags_ResizeX); // 150 width for menu
            {
                // List of menu items
                if (ImGui::Selectable("Project", selected_option == 0)) { selected_option = 0; }
                if (ImGui::Selectable("Appearance", selected_option == 1)) { selected_option = 1; }
                if (ImGui::Selectable("Physics", selected_option == 2)) { selected_option = 2; }
                if (ImGui::Selectable("Physics 2D", selected_option == 3)) { selected_option = 3; }
                if (ImGui::Selectable("Debugging", selected_option == 4)) { selected_option = 4; }
            }
            ImGui::EndChild();

            ImGui::SameLine(); // Move to the right for content

            // Define a child window for the right content area
            ImGui::BeginChild("RightContent", {0.0f, 0.0f}); // Fill remaining space
            {
                if (selected_option == 0) // Project
                {
                    ImGui::Text("Project");
                    ImGui::Separator();

                    if (Project::GetActive())
                    {
                        const std::string &project_path = Project::GetActiveProjectPath().generic_string();
                        const std::string &relative_path = std::filesystem::relative(project_path).generic_string();
                        const UUID start_scene = Project::GetActive()->GetConfig().StartScene;
                        ImGui::Text("Project Path %s", project_path.c_str());
                        ImGui::Text("Project Relative Path %s", relative_path.c_str());
                        ImGui::Text("Start Scene %llu", start_scene);
                    }
                }
                else if (selected_option == 1) // Appearance
                {
                    ImGui::Text("Appearance");
                    ImGui::Separator();

                    if (ImGui::BeginCombo("Themes", m_Themes.GetCurrentTheme().c_str()))
                    {
                        for (const auto &[key, value] : m_Themes.GetThemes())
                        {
                            const bool isSelected = m_Themes.GetCurrentTheme() == key;
                            if (ImGui::Selectable(key.c_str(), isSelected))
                                m_Themes.ApplyTheme(key);
                            if (isSelected)
                                ImGui::SetItemDefaultFocus();
                        }
                        ImGui::EndCombo();
                    }
                }
                else if (selected_option == 2) // Physics
                {
                    ImGui::Text("Physics");
                    ImGui::Separator();
                }
                else if (selected_option == 3) // Physics 2D
                {
                    ImGui::Text("Physics 2D");
                    ImGui::Separator();
                }
                else if (selected_option == 4) // Debugging
                {
                    ImGui::Text("Debugging");
                    ImGui::Separator();
                }
            }
            ImGui::EndChild();

            ImGui::End();
        }

        if (GuiRenderSettingsWindow)
        {
            static ImGuiTreeNodeFlags treeFlags = ImGuiTreeNodeFlags_OpenOnDoubleClick;

            ImGui::Begin("Render Settings", &GuiRenderSettingsWindow);
            if (ImGui::BeginTabBar("##render_settings"))
            {
                if (ImGui::BeginTabItem("Render Settings"))
                {
                    UI::DrawCheckbox("Visualize Collider", &m_VisualizeCollider);
                    UI::DrawCheckbox("Visualize Bounding Box", &m_VisualizeBoundingBox);

                    if (ImGui::TreeNodeEx("Shaders", treeFlags | ImGuiTreeNodeFlags_DefaultOpen))
                    {
                        if (ImGui::BeginTable("SHADERS_TABLE", 3))
                        {
                            ImGui::TableSetupScrollFreeze(0, 1);
                            ImGui::TableSetupColumn("Name");
                            ImGui::TableSetupColumn("Type");
                            ImGui::TableSetupColumn("Reload");
                            ImGui::TableHeadersRow();

                            for (auto &[name, shader] : Renderer::GetShaderLibrary().GetMap())
                            {
                                ImGui::PushID(name.c_str());
                                ImGui::TableNextRow();
                                ImGui::TableNextColumn();
                                ImGui::TextWrapped("%s", name.c_str());
                                ImGui::TableNextColumn();
                                ImGui::TextWrapped(shader->IsSPIRV() ? "SPIR-V" : "-");
                                ImGui::TableNextColumn();
                                if (ImGui::Button("Reload"))
                                {
                                    shader->Reload();
                                }
                                ImGui::PopID();
                            }
                            ImGui::EndTable();
                        }
                        ImGui::TreePop();
                    }

                    if (ImGui::TreeNodeEx("Camera Settings", treeFlags | ImGuiTreeNodeFlags_DefaultOpen))
                    {
                        ImGui::Text("Viewport Size %.0f, %.0f", m_SceneViewportSize.x, m_SceneViewportSize.y);

                        // Projection Type Settings
                        const char *CMPTypeString[] = { "Perspective", "Orthographic" };
                        const char *currentCMPType = CMPTypeString[static_cast<int>(m_EditorCamera.GetProjectionType())];
                        if (ImGui::BeginCombo("Projection", currentCMPType))
                        {
                            for (int i = 0; i < 2; i++)
                            {
                                const bool isSelected = currentCMPType == CMPTypeString[i];
                                if (ImGui::Selectable(CMPTypeString[i], isSelected))
                                {
                                    currentCMPType = CMPTypeString[i];
                                    m_EditorCamera.SetProjectionType(static_cast<ProjectionType>(i));

                                } if (isSelected) ImGui::SetItemDefaultFocus();
                            } ImGui::EndCombo();
                        }


                        // Projection settings
                        switch (m_EditorCamera.GetProjectionType())
                        {
                        case ProjectionType::Perspective:
                        {
                            const char *CMSTypeString[] = { "Pivot", "Free Move" };
                            const char *currentCMSTypeString = CMSTypeString[static_cast<int>(m_EditorCamera.GetStyle())];
                            if (ImGui::BeginCombo("Camera Style", currentCMSTypeString))
                            {
                                for (int i = 0; i < 2; i++)
                                {
                                    const bool isSelected = currentCMSTypeString == CMSTypeString[i];
                                    if (ImGui::Selectable(CMSTypeString[i], isSelected))
                                    {
                                        currentCMSTypeString = CMSTypeString[i];
                                        m_EditorCamera.SetStyle(static_cast<CameraStyle>(i));

                                    } if (isSelected) ImGui::SetItemDefaultFocus();
                                }
                                ImGui::EndCombo();
                            }

                            float fov = m_EditorCamera.GetFOV();
                            if (UI::DrawFloatControl("FOV", &fov, 1.0f, 20.0f, 120.0f))
                            {
                                m_EditorCamera.SetFov(fov);
                            }

                            UI::DrawCheckbox("Grid 3D", &m_Draw3DGrid);
                            if (m_Draw3DGrid)
                            {
                                UI::DrawIntControl("Grid Size", &m_3DGridSize, 1.0f);
                            }
                            break;
                        }
                        case ProjectionType::Orthographic:
                            UI::DrawCheckbox("Grid 2D", &m_Draw2DGrid);
                            break;
                        }

                        ImGui::ColorEdit4("Background", glm::value_ptr(m_ClearColor));
                        ImGui::TreePop();
                    }

                    if (ImGui::TreeNodeEx("Statistics", treeFlags | ImGuiTreeNodeFlags_DefaultOpen))
                    {
                        const auto renderStats = Renderer::GetStatistics();
                        ImGui::Text("Draw Calls: %d", renderStats.DrawCalls);
                        ImGui::Text("Quads: %d", renderStats.QuadCount);
                        ImGui::Text("Circles: %d", renderStats.CircleCount);
                        ImGui::Text("Lines: %d", renderStats.LineCount);
                        ImGui::Text("Cubes: %d", renderStats.CubeCount);
                        ImGui::Text("Vertices: %d", renderStats.GetTotalQuadVertexCount());
                        ImGui::Text("Indices: %d", renderStats.GetTotalQuadIndexCount());
                        ImGui::Text("Viewport Hovered (%d)", IsViewportHovered);
                        ImGui::TreePop();
                    }

                    ImGui::EndTabItem();
                }

                if (ImGui::BeginTabItem("Render Time"))
                {
                    int h = ((int)m_Time / 3600) % 60;
                    int m = ((int)m_Time / 60) % 60;
                    int s = (int)m_Time % 60;

                    char timer[34];
                    strcpy(timer, "Total Working Timer: %d : %d : %d");
                    ImGui::Text(timer, h, m, s);

                    if (ImGui::Button("Reset Time"))
                    {
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

        if (GuiMenuStyleWindow)
        {
            ImGui::Begin("Style Editor", &GuiMenuStyleWindow);
            ImGui::ShowStyleEditor();
            ImGui::End();
        }

        if (GuiImGuiDemoWindow)
        {
            ImGui::ShowDemoWindow(&GuiImGuiDemoWindow);
        }
    }

    void EditorLayer::ConsoleWindow()
    {
        bool scrollToBottom = false;
        static bool autoScroll = true;
        static size_t lastMessageCount = 0;

        if (GuiConsoleWindow)
        {
            ImGui::Begin("Console", &GuiConsoleWindow);

            static ImGuiWindowFlags windowFlags = ImGuiWindowFlags_NoScrollWithMouse | ImGuiWindowFlags_NoScrollbar;
            ImGui::BeginChild("navigation_button", ImVec2(ImGui::GetContentRegionAvail().x, 25.0f), 0, windowFlags);
            if (UI::DrawButton("Clear"))
            {
                ConsoleManager::Get().Clear();
                lastMessageCount = 0;
            }
            ImGui::SameLine();
            if (ImGui::Checkbox("Auto-scroll", &autoScroll))
            {
                lastMessageCount = 0;
            }
            ImGui::EndChild();

            ImGui::BeginChild("CONSOLE_CONTENT", ImGui::GetContentRegionAvail(), 0, windowFlags);
            // By default, if we don't enable ScrollX the sizing policy for each column is "Stretch"
            // All columns maintain a sizing weight, and they will occupy all available width.
            static ImGuiTableFlags flags = ImGuiTableFlags_BordersOuterH | ImGuiTableFlags_BordersInnerH
                | ImGuiTableFlags_BordersOuterV | ImGuiTableFlags_RowBg | ImGuiTableFlags_ScrollY;

            if (ImGui::BeginTable("CONSOLE_TABLE", 3, flags))
            {
                ImGui::TableSetupScrollFreeze(0, 1);
                // Timestamp column: left-aligned, fixed size
                ImGui::TableSetupColumn("Timestamp", ImGuiTableColumnFlags_WidthFixed, 60.0f);
                // Message column: stretches
                ImGui::TableSetupColumn("Message", ImGuiTableColumnFlags_WidthStretch);
                // Type column: right-aligned, fixed size
                ImGui::TableSetupColumn("Type", ImGuiTableColumnFlags_WidthFixed, 40.0f);
                ImGui::TableHeadersRow();

                ImGuiListClipper clipper;
                const auto &messages = ConsoleManager::GetMessages();
                clipper.Begin(messages.size());

                // Check if there are new messages
                if (messages.size() > lastMessageCount && autoScroll)
                {
                    scrollToBottom = true;
                    lastMessageCount = messages.size();
                }

                while (clipper.Step())
                {
                    for (int row = clipper.DisplayStart; row < clipper.DisplayEnd; ++row)
                    {
                        ImGui::TableNextRow();

                        switch (messages[row].Level)
                        {
                        case LogLevel::Info:    ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.0f, 1.0f, 1.0f, 1.0f)); break;
                        case LogLevel::Error:   ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.8f, 0.0f, 0.0f, 1.0f)); break;
                        case LogLevel::Warning: ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.0f, 0.8f, 0.2f, 1.0f)); break;
                        }

                        ImGui::TableSetColumnIndex(0);
                        ImGui::Text("%s", messages[row].Timestamp.c_str());
                        ImGui::TableSetColumnIndex(1);
                        ImGui::Text("%s", messages[row].Message.c_str());
                        ImGui::TableSetColumnIndex(2);

                        switch (messages[row].Level)
                        {
                        case LogLevel::Info:    ImGui::Text("INFO"); break;
                        case LogLevel::Error:   ImGui::Text("ERROR"); break;
                        case LogLevel::Warning: ImGui::Text("WARN"); break;
                        }

                        ImGui::PopStyleColor(1);
                    }
                }
                if (scrollToBottom)
                {
                    ImGui::SetScrollHereY(1.0f);
                    scrollToBottom = false;
                }
                ImGui::EndTable();
            }

            ImGui::EndChild();

            ImGui::End();
        }
    }

    void EditorLayer::InitGrid()
    {
        glGenVertexArrays(1, &m_GridVAO);
        glGenBuffers(1, &m_GridVBO);
        glBindVertexArray(m_GridVAO);
        glBindBuffer(GL_ARRAY_BUFFER, m_GridVBO);
    }

    void EditorLayer::ShowGrid()
    {
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

        Renderer::GetShader("GridShader")->Enable();
        Renderer::GetShader("GridShader")->SetMatrix("viewProjection", m_EditorCamera.GetViewProjection());
        Renderer::GetShader("GridShader")->SetVector("cameraPosition", m_EditorCamera.GetPosition());
        Renderer::GetShader("GridShader")->SetVector("thinColor", m_GridThinColor);
        Renderer::GetShader("GridShader")->SetVector("thickColor", m_GridThickColor);

        glBindVertexArray(m_GridVAO);
        glDrawArrays(GL_TRIANGLES, 0, 6);
        glBindVertexArray(0);

        Renderer::GetShader("GridShader")->Disable();
    }

    bool EditorLayer::OnMouseButtonPressed(MouseButtonPressedEvent &e)
    {
        OGN_PROFILER_INPUT();

        if (IsViewportHovered && !IsViewportFocused)
        {
            if (e.Is(Mouse::ButtonMiddle) || e.Is(Mouse::ButtonRight) || e.Is(Mouse::ButtonLeft))
            {
                ImGui::SetWindowFocus("Scene");
            }
        }

        if (e.Is(Mouse::ButtonLeft) && IsViewportHovered && !ImGuizmo::IsOver() && m_SceneHierarchy.GetContext())
        {
            const glm::vec2 viewportSize = m_SceneViewportBounds[1] - m_SceneViewportBounds[0];
            const glm::vec2 &mouse = { m_ViewportMousePos.x, m_ViewportMousePos.y };

            float closestT = std::numeric_limits<float>::max();
            Entity closestEntity = { entt::null, nullptr };

            auto ray_direction = glm::vec3(0.0f);
            auto ray_origin = glm::vec3(0.0f);

            ray_direction = Math::GetRayFromScreenCoords(mouse, viewportSize,
                                                        m_EditorCamera.GetProjectionMatrix(),
                                                        m_EditorCamera.GetViewMatrix(),
                                                        m_EditorCamera.IsPerspective(),
                                                        ray_origin);

            const auto view = m_ActiveScene->m_Registry.view<TransformComponent>();
            for (auto [entity, tc] : view.each())
            {
                OBB obb = OBB(tc.WorldTranslation, tc.WorldScale, tc.WorldRotation);
                if (m_ActiveScene->m_Registry.any_of<TextComponent>(entity))
                {
                    auto text = m_ActiveScene->m_Registry.get<TextComponent>(entity);
                    glm::vec3 scale = tc.WorldScale * glm::vec3(text.Size.x, -text.Size.y, 1.0f);
                    obb = OBB(tc.WorldTranslation, scale, tc.WorldRotation);
                }

                float t_intersect;
                if (obb.RayIntersection(ray_origin, ray_direction, t_intersect))
                {
                    // find the closest entity
                    if (t_intersect < closestT)
                    {
                        closestT = t_intersect;
                        closestEntity = { entity, m_ActiveScene.get() };
                    }
                }
            }

            m_SceneHierarchy.SetSelectedEntity(closestEntity);
            if (!closestEntity)
            {
                m_Gizmos->SetType(GizmoType::NONE);
            }
        }

        return false;
    }

    void EditorLayer::InputProcedure(Timestep time)
    {
        if (m_SceneState == SceneState::Play || !(IsViewportHovered && IsViewportFocused)  || !m_SceneHierarchy.GetContext())
        {
            return;
        }

        const glm::vec2 mouse { Input::Get().GetMouseX(), Input::Get().GetMouseY() };
        const glm::vec2 delta = Input::Get().GetMouseDelta();

        if (Input::Get().IsMouseButtonPressed(Mouse::ButtonLeft))
        {
            Entity selectedEntity = m_SceneHierarchy.GetSelectedEntity();

            if (selectedEntity && IsViewportFocused && IsViewportHovered)
            {
                if (m_EditorCamera.GetProjectionType() == ProjectionType::Orthographic && !ImGuizmo::IsUsing())
                {
                    auto &tc = selectedEntity.GetComponent<TransformComponent>();
                    const auto &idc = selectedEntity.GetComponent<IDComponent>();
                    float viewport_height = m_EditorCamera.GetViewportSize().y;
                    float orthographic_scale = m_EditorCamera.GetOrthoScale() / viewport_height;
                    if (m_SceneState == SceneState::Play)
                    {
                        if (Entity cam = m_ActiveScene->GetPrimaryCameraEntity())
                        {
                            const auto &cc = cam.GetComponent<CameraComponent>();
                            orthographic_scale = cc.Camera.GetOrthoScale() / cc.Camera.GetViewportSize().y;
                        }
                    }

                    static glm::vec3 translation = tc.Translation;
                    if (selectedEntity.HasComponent<Rigidbody2DComponent>() && m_SceneState != SceneState::Edit)
                    {
                        auto velocity = glm::vec3(0.0f);
                        Rigidbody2DComponent &body = selectedEntity.GetComponent<Rigidbody2DComponent>();
                        if (selectedEntity.HasParent())
                        {
                            Entity parent = m_ActiveScene->GetEntityWithUUID(idc.Parent);
                            const auto &parent_tc = parent.GetComponent<TransformComponent>();
                            velocity += inverse(parent_tc.WorldRotation) * glm::vec3(delta.x * orthographic_scale * 0.5f, -delta.y * orthographic_scale * 0.5f, 0.0f);
                        }
                        else
                        {
                            velocity += glm::vec3(delta.x * orthographic_scale * 0.5f, -delta.y * orthographic_scale * 0.5f, 0.0f);
                        }

                        b2Body_SetLinearVelocity(body.BodyId, b2Vec2(velocity.x, velocity.y));
                    }
                    else
                    {
                        if (Input::Get().IsKeyPressed(Key::LeftShift))
                        {
                            float snap_value = 0.5f;
                            if (Input::Get().IsKeyPressed(Key::LeftControl))
                            {
                                snap_value = 0.1f;
                            }

                            if (selectedEntity.HasParent())
                            {
                                Entity parent = m_ActiveScene->GetEntityWithUUID(idc.Parent);
                                auto &parent_tc = parent.GetComponent<TransformComponent>();
                                translation += inverse(parent_tc.WorldRotation) * glm::vec3(delta.x * orthographic_scale, -delta.y * orthographic_scale, 0.0f);
                            }
                            else
                            {
                                translation += glm::vec3(delta.x * orthographic_scale, -delta.y * orthographic_scale, 0.0f);
                            }

                            tc.Translation.x = round(translation.x / snap_value) * snap_value;
                            tc.Translation.y = round(translation.y / snap_value) * snap_value;
                        }
                        else
                        {
                            translation = glm::vec3(delta.x * orthographic_scale, -delta.y * orthographic_scale, 0.0f);

                            if (selectedEntity.HasParent())
                            {
                                Entity parent = m_ActiveScene->GetEntityWithUUID(idc.Parent);
                                auto &parent_tc = parent.GetComponent<TransformComponent>();

                                translation = inverse(parent_tc.WorldRotation) * translation;
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

        auto &app = Application::Instance();
        const bool control = Input::Get().IsKeyPressed(Key::LeftControl) || Input::Get().IsKeyPressed(Key::RightControl);
        const bool shift = Input::Get().IsKeyPressed(Key::LeftShift) || Input::Get().IsKeyPressed(Key::RightShift);

        ImGuiIO& io = ImGui::GetIO();
        Entity selectedEntity = m_SceneHierarchy.GetSelectedEntity();

        switch (e.GetKeyCode())
        {
        case Key::Escape:
        {
            if (m_SceneState == SceneState::Play)
            {
                m_ActiveScene->UnlockMouse();
            }
            else 
            {
                if((IsViewportFocused))
                    m_Gizmos->SetType(GizmoType::NONE);
            }
            break;
        }
        case Key::D:
        {
            if (control && 
                !(m_SceneState == SceneState::Play || m_SceneState == SceneState::Simulate))
            {
                OnDuplicateEntity();
            }
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
        case Key::F5:
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
            break;
        }

        case Key::F6:
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
            if (!io.WantTextInput)
            {
                if (!ImGuizmo::IsUsing() && !m_EditorCamera.IsPerspective())
                {
                    m_Gizmos->SetType(GizmoType::BOUNDARY2D);
                }

                if (control)
                {
                    CommandManager::Instance().Redo();
                }
            }
            break;
        }

        case Key::F11:
        {
            app.GetWindow().ToggleFullScreen();
            break;
        }

        case Key::Delete:
        {
            if (!io.WantTextInput)
                OnDestroyEntity();
            break;
        }
        case Key::Z:
        {
            if (!io.WantTextInput && control)
            {
                CommandManager::Instance().Undo();
            }
            break;
        }
        }

        return false;
    }
}
