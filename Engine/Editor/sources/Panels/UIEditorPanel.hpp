// Copyright (c) Evangelion Manuhutu | ORigin Engine

#ifndef UI_EDITOR_PANEL_HPP
#define UI_EDITOR_PANEL_HPP

#include "Origin/Core/Event.h"
#include "Origin/Scene/Entity.h"
#include "Origin/Core/KeyEvent.h"
#include "Origin/Scene/Camera/EditorCamera.h"
#include "Origin/Scene/Scene.h"

#include "PanelBase.hpp"
#include "Origin/Core/Input.h"

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
    bool OnMouseMoved(MouseMovedEvent &e);

    void OnMouse(float ts);

    void Open() override;
    void Close() override;

    void Reset() override;

    static UIEditorPanel *GetInstance();

    bool IsViewportFocused = false;
    bool IsViewportHovered = false;

private:

    Scene *m_scene = nullptr;
    EditorCamera m_Camera;
    UIComponent *m_ui_comp = nullptr;

    Ref<Framebuffer> m_framebuffer;
    glm::vec2 m_move_translation = { 0.0f, 0.0f };
    i32 m_selected_index = -1, m_hovered_index = -1;
};
    
}

#endif