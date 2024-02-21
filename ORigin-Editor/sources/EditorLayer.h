// Copyright (c) Evangelion Manuhutu | ORigin Engine
#pragma once
#include <Origin.h>
#include "Panels/Dockspace.h"
#include "Panels/SceneHierarchyPanel.h"
#include "Panels/ContentBrowserPanel.h"
#include "Panels/AnimationTimeline.h"

#include <ImGuizmo.h>

namespace origin {

  class EditorLayer : public Layer
  {
  public:
		EditorLayer();
		~EditorLayer();

		void OnAttach() override;
		void OnUpdate(Timestep ts) override;
		SceneHierarchyPanel& GetSceneHierarchy() { return m_SceneHierarchy; }
		inline static EditorLayer& Get() { return *s_Instance; }

    bool guiDockingSpaceOpen = true;
    bool guiMenuFullscreen = false;
    bool guiMenuStyle = false;
    bool guiPrompt = false;
    bool guiRenderSettingsWindow = true;
    bool guiDebugInfo = true;
    bool guiImGuiDemoWindow = false;
    bool guiOverlay = true;
    bool guiAudioLibraryWindow = false;
    bool guiAudioCreationWindow = false;
    bool guiAnimationWindow = true;

  private:
    void Draw(float deltaTime);
  	void SceneViewport();
    void SceneViewportToolbar();
    void SceneViewportMenu();
    bool NewProject();
    bool OpenProject();
    bool OpenProject(const std::filesystem::path& path);
    void SaveProject();
	  void MenuBar();
    void NewScene();
    void SaveScene();
    void SaveSceneAs();
    void OpenScene();
    void OpenScene(AssetHandle handle);
    void SerializeScene(std::shared_ptr<Scene> scene, const std::filesystem::path filepath);
    void OnDuplicateEntity();
    bool OnWindowDrop(WindowDropEvent& e);
    bool OnWindowResize(WindowResizeEvent& e);
    bool OnMouseMovedEvent(MouseMovedEvent& e);
    bool OnMouseButtonEvent(MouseButtonEvent& e);
    void OnEvent(Event& e) override;
    bool OnKeyPressed(KeyPressedEvent& e);
    bool OnMouseButtonPressed(MouseButtonPressedEvent& e);
    void OnGuiRender() override;
    void OnScenePlay();
    void OnScenePause();
    void OnSceneSimulate();
    void OnSceneStop();
    void InputProcedure(Timestep time);
    void GUIRender();

		enum class SceneState
		{
			Edit = 0,
			Play = 1,
			Simulate = 2
		};

		static EditorLayer* s_Instance;
		SceneState m_SceneState = SceneState::Edit;
		enum ViewportMenuContext { CreateMenu = 0, EntityProperties = 1 };
		ViewportMenuContext m_VpMenuContext = ViewportMenuContext::CreateMenu;
		SceneHierarchyPanel m_SceneHierarchy;
		Dockspace m_Dockspace;
		EditorCamera m_EditorCamera;
		ShaderLibrary m_ShaderLibrary;
		AnimationTimeline m_AnimationTimeline;
		std::unordered_map<std::string, std::shared_ptr<Texture2D>> m_UITextures;
		std::shared_ptr<Texture2D> m_OriginEngineTex;
		std::shared_ptr<Framebuffer> m_Framebuffer, m_GameFramebuffer;
		std::unique_ptr<ContentBrowserPanel> m_ContentBrowser;
		std::shared_ptr<Scene> m_ActiveScene, m_EditorScene;
		std::filesystem::path m_ScenePath, m_ProjectDirectoryPath;

		glm::vec4 clearColor = glm::vec4(0.1f, 0.1f, 0.1f, 1.0f);
		glm::vec2 m_GameViewportSize = { 0.0f, 0.0f };
		glm::vec2 m_GameViewportBounds[2] = { glm::vec2(0.0f), glm::vec2(0.0f) };
		glm::vec2 m_SceneViewportSize = { 0.0f, 0.0f };
		glm::vec2 m_SceneViewportBounds[2] = { glm::vec2(0.0f), glm::vec2(0.0f) };
		glm::vec3 cameraPosition = {};
		glm::vec2 m_InitialMousePosition = glm::vec2(0.0f);

		int lastMouseX = 0, mouseX = 0;
		int lastMouseY = 0, mouseY = 0;
		int m_GizmosType = -1;
		int m_GizmosMode = 0;
		int m_PixelData = -1;
		int m_RenderTarget = 0;
		float m_CameraFov = 45.0f;
		float cameraYaw = 0.0f, cameraPitch = 0.0f;
		float m_GameViewportSizeX = 0.0f, m_GameViewportSizeY = 0.0f;
		float m_Time = 0.0f;
		bool drawLineMode = false;
		bool m_VisualizeCollider = true;
		bool m_SceneViewportHovered = false;
		bool m_SceneViewportFocused = false;
		bool VpMenuContextActive;
		Entity m_HoveredEntity = {};

    friend class Gizmos;
    friend class SceneHierarchyPanel;
  };
}