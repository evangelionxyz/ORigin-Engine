// Copyright (c) Evangelion Manuhutu | ORigin Engine
#pragma once

#include <Origin.h>
#include "Gizmos/Gizmos.h"
#include "Panels/UIEditor.h"
#include "Panels/Dockspace.h"
#include "Panels/MaterialEditor.h"
#include "Panels/AnimationTimeline.h"
#include "Panels/SpriteSheetEditor.h"
#include "Panels/SceneHierarchyPanel.h"
#include "Panels/ContentBrowserPanel.h"

#include "Themes.h"

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
        SceneHierarchyPanel& GetSceneHierarchy() { return m_SceneHierarchy; }
        static EditorLayer &Get();

        bool GuiVSync = true;
        bool GuiMenuFullscreen = false;
        bool GuiMenuStyleWindow = false;
        bool GuiRenderSettingsWindow = true;
        bool GuiImGuiDemoWindow = false;
        bool GuiConsoleWindow = true;
        bool GuiOptionsWindow = false;

        bool IsViewportHovered = false;
        bool IsViewportFocused = false;

    private:
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
        void OnDestroyEntity();
        void OnEvent(Event& e) override;
        bool OnKeyPressed(KeyPressedEvent& e);
        bool OnMouseButtonPressed(MouseButtonPressedEvent& e);
        void OnGuiRender() override;
        void OnScenePlay();
        void OnScenePause() const;
        void OnSceneSimulate();
        void OnSceneStop();
        void InputProcedure(Timestep time);
        void GUIRender();
        void ConsoleWindow();

        void InitGrid();
        void ShowGrid();

        enum class SceneState
        {
            Edit = 0,
            Play = 1,
            Simulate = 2
        };

        std::vector<ProfilerResult> m_ProfilerResults;

        SceneState m_SceneState = SceneState::Edit;
        SceneHierarchyPanel m_SceneHierarchy;
        Dockspace m_Dockspace;
        EditorCamera m_EditorCamera;
        MaterialEditor m_MaterialEditor;
        Themes m_Themes;
        Scope<SpriteSheetEditor> m_SpriteSheetEditor;
        Scope<UIEditor> m_UIEditor;
        Scope<Gizmos> m_Gizmos;
        u32 m_GridVAO, m_GridVBO;
        glm::vec4 m_GridThinColor = glm::vec4(0.5f, 0.5f, 0.5f, 1.0f);
        glm::vec4 m_GridThickColor = glm::vec4(0.5f, 0.5f, 0.5f, 1.0f);

        std::unordered_map<std::string, Ref<Texture2D>> m_UITextures;
        Ref<Texture2D> m_OriginEngineTex;
        Ref<Framebuffer> m_Framebuffer, m_GameFramebuffer;
        Scope<ContentBrowserPanel> m_ContentBrowser;
        Ref<Scene> m_ActiveScene, m_EditorScene;
        std::filesystem::path m_ScenePath, m_ProjectDirectoryPath;

        glm::vec4 m_ClearColor = glm::vec4(0.1f, 0.1f, 0.1f, 1.0f);
        glm::vec2 m_GameViewportSize = { 0.0f, 0.0f };
        glm::vec2 m_SceneViewportSize = { 0.0f, 0.0f };
        glm::vec2 m_ViewportMousePos = { 0.0f, 0.0f };
        glm::vec2 m_SceneViewportBounds[2] = { glm::vec2(0.0f), glm::vec2(0.0f) };

        ImGuizmo::OPERATION m_ImGuizmoOperation = (ImGuizmo::OPERATION)0;
        int m_GizmosMode = 0;
        int m_3DGridSize = 30;
        int m_RenderTarget = 0;
        float m_Time = 0.0f;
        bool m_Draw3DGrid = false;
        bool m_Draw2DGrid = true;
        bool m_DrawLineModeActive = false;
        bool m_VisualizeCollider = false;
        bool m_VisualizeBoundingBox = false;
        Entity m_HoveredEntity = {};

        friend class Gizmos;
        friend class UIEditor;
        friend class ContentBrowserPanel;
        friend class SceneHierarchyPanel;
        friend class EditorSerializer;
    };
}