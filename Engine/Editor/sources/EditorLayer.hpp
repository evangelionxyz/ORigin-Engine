// Copyright (c) Evangelion Manuhutu | ORigin Engine

#ifndef EDITOR_LAYER_HPP
#define EDITOR_LAYER_HPP

#include <Origin.hpp>
#include "Gizmos.hpp"
#include "Themes.hpp"
#include "Dockspace.hpp"

#include "Origin/Utils/PlatformUtils.h"
#include "Origin/Scripting/ScriptEngine.h"
#include "Origin/Asset/AssetManager.h"
#include "Origin/Asset/AssetImporter.h"
#include "Origin/GUI/UI.h"

#include "Panels/AudioSystemPanel.hpp"
#include "Panels/BlendSpacePanel.hpp"
#include "Panels/UIEditorPanel.hpp"
#include "Panels/MaterialEditorPanel.hpp"
#include "Panels/AnimationTimeline.hpp"
#include "Panels/SpriteSheetEditorPanel.hpp"
#include "Panels/SceneHierarchyPanel.hpp"
#include "Panels/ContentBrowserPanel.hpp"

#include "Serializer/EditorSerializer.hpp"

#include <ImGuizmo.h>

namespace origin
{
    class EditorLayer : public Layer
    {
    public:
        EditorLayer();

        void OnAttach() override;
        void OnDetach() override;
        
        void OnUpdate(Timestep ts) override;

        static EditorLayer &Get();

        bool GuiVSync = true;
        bool GuiMenuFullscreen = false;
        bool GuiMenuStyleWindow = false;
        bool GuiRenderSettingsWindow = true;
        bool GuiImGuiDemoWindow = false;
        bool GuiConsoleWindow = true;
        bool GuiPreferencesWindow = false;

        bool IsViewportHovered = false;
        bool IsViewportFocused = false;

    private:
        void CreatePanels();
        void DestroyPanels();
        void AddPanel(PanelBase *panel);

        void SystemUpdate(Timestep ts);
        void Render(Timestep ts);
        void SceneViewport();
        void SceneViewportToolbar();
        bool NewProject();
        bool OpenProject();
        bool OpenProject(const std::filesystem::path& path);
        void MenuBar();
        void NewScene();
        void SaveScene();
        void SaveSceneAs();
        void OpenScene();
        void OpenScene(AssetHandle handle);
        void SerializeScene(const Ref<Scene> &scene, const std::filesystem::path &filepath);
        void OnDuplicateEntity();
        void OnDestroyEntity() const;
        void OnEvent(Event& e) override;
        bool OnKeyPressed(KeyPressedEvent &e);
        bool OnMouseButtonPressed(MouseButtonPressedEvent &e);
        bool OnMouseScroll(MouseScrolledEvent &e);
        bool OnMouseMove(MouseMovedEvent &e);

        void OnGuiRender() override;
        void OnScenePlay();
        void OnScenePause() const;
        void OnSceneSimulate();
        void OnSceneStop();
        void InputProcedure(Timestep time);
        void GUIRender();
        void ConsoleWindow();
        void DisplayMemoryGraphUsage();
        void DisplayCPUUsageGraph();

        void InitGrid();
        void ShowGrid();

        enum class SceneState : u8
        {
            Edit = 0,
            Play = 1,
            Simulate = 2
        };

        std::vector<ProfilerResult> m_ProfilerResults;
        SceneState m_SceneState = SceneState::Edit;
        EditorCamera m_EditorCamera;

        // ================ Panels ==================
        SceneHierarchyPanel     *m_SceneHierarchyPanel    = nullptr;
        SpriteSheetEditorPanel  *m_SpriteSheetEditorPanel = nullptr;
        UIEditorPanel           *m_UIEditorPanel          = nullptr;
        BlendSpacePanel         *m_BlendSpacePanel        = nullptr;
        Scope<ContentBrowserPanel> m_ContentBrowser;
        Scope<Gizmos> m_gizmo;
        std::vector<PanelBase *> m_Panels;
        // ==========================================
        // ==========================================
        

        std::unordered_map<std::string, Ref<Texture2D>> m_UITextures;
        Ref<Texture2D> m_OriginEngineTex;
        Ref<Framebuffer> m_Framebuffer, m_GameFramebuffer;
        Ref<Scene> m_ActiveScene, m_EditorScene;
        GuiWindow m_GuiWindowSceneStats;

        std::filesystem::path m_ScenePath, m_ProjectDirectoryPath;
        glm::vec4 m_GridThinColor = glm::vec4(0.5f, 0.5f, 0.5f, 1.0f);
        glm::vec4 m_GridThickColor = glm::vec4(0.5f, 0.5f, 0.5f, 1.0f);
        glm::vec4 m_ClearColor = glm::vec4(0.1f, 0.1f, 0.1f, 1.0f);
        glm::vec2 m_GameViewportSize = { 0.0f, 0.0f };
        glm::vec2 m_ViewportMousePos = { 0.0f, 0.0f };
        Rect m_viewport_rect = Rect();
        Entity m_HoveredEntity = {};

        ImGuizmo::OPERATION m_gizmo_operation = static_cast<ImGuizmo::OPERATION>(0);
        i32 m_gizmo_mode = 0;
        i32 m_RenderTarget = 0;
        f32 m_Time = 0.0f;
        u32 m_GridVAO, m_GridVBO;
        bool m_Draw2DGrid = true;
        bool m_DrawLineModeActive = false;
        bool m_VisualizeCollider = false;
        bool m_VisualizeBoundingBox = false;
        bool m_IsProjectBrowserOpen = true;

        friend class Gizmos;
        friend class UIEditorPanel;
        friend class ContentBrowserPanel;
        friend class SceneHierarchyPanel;
        friend class EditorSerializer;
        friend class ProjectBrowser;
    };
}

#endif