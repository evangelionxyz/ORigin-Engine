// Copyright (c) 2022 Evangelion Manuhutu | ORigin Engine

#pragma once
#include <Origin.h>
#include "panels\Dockspace.h"
#include "panels\SceneHierarchyPanel.h"
#include "panels\ContentBrowserPanel.h"

namespace Origin {

  class Editor : public Layer
  {
  public:
    Editor();
    void OnAttach() override;
    void OnUpdate(Timestep ts) override;

  private:
    void ViewportToolbar();
    void ViewportMenu();
    void NewScene();
    void SaveSceneAs();
    void OpenScene(const std::filesystem::path& path);
    void OpenScene();

    void VpGui();
    void MenuBar();

    static bool OnWindowResize(WindowResizeEvent& e);
    static bool OnMouseMovedEvent(MouseMovedEvent& e);
    static bool OnMouseButtonEvent(MouseButtonEvent& e);

    void OnEvent(Event& e) override;
    bool OnKeyPressed(KeyPressedEvent& e);
    bool OnMouseButtonPressed(MouseButtonPressedEvent& e);
    void InputProccedure(float time);
    void OnGuiRender() override;

    void OnScenePlay();
    void OnSceneStop();

    Entity m_HoveredEntity = {};
    Entity m_SelectedEntity = {};

    glm::vec4 clearColor = glm::vec4(0.1f, 0.1f, 0.1f, 1.0f);
    glm::vec4 color = glm::vec4(1.0f, 0.0f, 1.0f, 1.0f);
    glm::vec2 position = glm::vec2(0);
    glm::vec2 scale = glm::vec2(1);

    glm::vec2 m_ViewportSize = { 0.0f, 0.0f };
    glm::vec2 m_ViewportBounds[2] = { glm::vec2(0.0f), glm::vec2(0.0f) };
    glm::vec3 cameraPosition = {};
  	int lastMouseX = 0, mouseX = 0;
    int lastMouseY = 0, mouseY = 0;
	  int m_GizmosType = -1;
    int m_GizmosMode = 0;
    int m_PixelData = -1;
    int m_RenderTarget = 0;

    float cameraYaw = 0.0f, cameraPitch = 0.0f;
    float m_Time = 0.0f;

    bool drawLineMode = false;
    bool m_ViewportHovered = false;

    std::shared_ptr<Texture2D> m_PlayButton, m_StopButton;

    EditorCamera m_EditorCamera;
    ShaderLibrary m_ShaderLibrary;
    std::shared_ptr<Framebuffer> m_Framebuffer;
    std::shared_ptr<Scene> m_ActiveScene;

    enum class SceneState { Edit = 0, Play = 1 };
    SceneState m_SceneState = SceneState::Edit;

    // panels
    Dockspace m_Dockspace;
    ContentBrowserPanel m_ContentBrowser;
    SceneHierarchyPanel m_SceneHierarchy;

    enum ViewportMenuContext { CreateMenu = 0, EntityProperties = 1 };
    ViewportMenuContext m_VpMenuContext = ViewportMenuContext::CreateMenu;
    bool VpMenuContextActive;
  };
}