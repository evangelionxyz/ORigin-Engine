// Copyright (c) Evangelion Manuhutu | ORigin Engine

#ifndef UI_EDITOR_PANEL_HPP
#define UI_EDITOR_PANEL_HPP

#include "Origin/Core/Event.h"
#include "Origin/Scene/Entity.h"
#include "Origin/Core/KeyEvent.h"
#include "Origin/Scene/Camera/EditorCamera.h"
#include "Origin/Scene/Scene.h"

#include "PanelBase.hpp"

namespace origin {
class UIComponent;
class UIEditorPanel : public PanelBase
{
public:
    UIEditorPanel(Scene *scene, bool open = false);
    void SetContext(Scene *scene);
    void SetActive(UIComponent *component);
    void CreateNewText();
    void CreateNewTexture();
    bool RenameComponent(int index, const std::string &newName);

    void Render() override;
    void OnUpdate(float delta_time) override;

    void OnEvent(Event &e) override;
    bool OnMouseButtonPressed(MouseButtonPressedEvent &e);
    bool OnMouseScroll(MouseScrolledEvent &e);
    bool OnKeyPressed(KeyPressedEvent &e);

    void OnMouse(float ts);

    void Open();

    static UIEditorPanel *GetInstance();

    bool IsViewportFocused = false;
    bool IsViewportHovered = false;

private:
    void DrawInspector();
    void DrawHierarchy();

    Scene *m_Scene = nullptr;
    EditorCamera m_Camera;
    UIComponent *m_UICompHandler = nullptr;

    Ref<Framebuffer> m_Framebuffer;
    Rect m_ViewportRect = Rect();

    glm::ivec2 m_Mouse = { 0, 0 };
    glm::vec2 m_MoveTranslation = { 0.0f, 0.0f };
    int m_SelectedIndex = 0;
};

}

#endif