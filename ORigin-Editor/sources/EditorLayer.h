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

#include <ImGuizmo.h>

namespace origin
{
	class EditorLayer : public Layer
	{
	public:
		EditorLayer();
		~EditorLayer();

		void OnAttach() override;
		void OnUpdate(Timestep ts) override;
		SceneHierarchyPanel& GetSceneHierarchy() { return m_SceneHierarchy; }
		static EditorLayer &Get();

		bool guiStatisticWindow = true;
		bool guiMenuFullscreen = false;
		bool guiMenuStyle = false;
		bool guiRenderSettingsWindow = true;
		bool guiImGuiDemoWindow = false;
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
		void SerializeScene(const std::shared_ptr<Scene> &scene, const std::filesystem::path &filepath);
		void OnDuplicateEntity();
		void OnDestroyEntity();
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

		std::vector<ProfilerResult> m_ProfilerResults;

		SceneState m_SceneState = SceneState::Edit;
		SceneHierarchyPanel m_SceneHierarchy;
		Dockspace m_Dockspace;
		EditorCamera m_EditorCamera;
		MaterialEditor m_MaterialEditor;
		std::unique_ptr<SpriteSheetEditor> m_SpriteSheetEditor;
		std::unique_ptr<UIEditor> m_UIEditor;
		std::unique_ptr<Gizmos> m_Gizmos;

		std::unordered_map<std::string, std::shared_ptr<Texture2D>> m_UITextures;
		std::shared_ptr<Texture2D> m_OriginEngineTex;
		std::shared_ptr<Framebuffer> m_Framebuffer, m_GameFramebuffer;
		std::unique_ptr<ContentBrowserPanel> m_ContentBrowser;
		std::shared_ptr<Scene> m_ActiveScene, m_EditorScene;
		std::filesystem::path m_ScenePath, m_ProjectDirectoryPath;

		glm::vec4 m_ClearColor = glm::vec4(0.1f, 0.1f, 0.1f, 1.0f);
		glm::vec2 m_GameViewportSize = { 0.0f, 0.0f };
		glm::vec2 m_SceneViewportSize = { 0.0f, 0.0f };
		glm::vec2 m_ViewportMousePos = { 0.0f, 0.0f };
		glm::vec2 m_SceneViewportBounds[2] = { glm::vec2(0.0f), glm::vec2(0.0f) };

		ImGuizmo::OPERATION m_ImGuizmoOperation = (ImGuizmo::OPERATION)0;
		int m_GizmosMode = 0;
		int m_3DGridSize = 30;
		int m_PixelData = -1;
		int m_RenderTarget = 0;
		float m_Time = 0.0f;
		bool m_Draw3DGrid = true;
		bool m_Draw2DGrid = true;
		bool m_DrawLineModeActive = false;
		bool m_VisualizeCollider = false;
		Entity m_HoveredEntity = {};

		friend class Gizmos;
		friend class UIEditor;
		friend class ContentBrowserPanel;
		friend class SceneHierarchyPanel;
	};
}