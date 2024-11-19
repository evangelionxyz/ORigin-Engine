// Copyright (c) Evangelion Manuhutu | ORigin Engine

#pragma once
#include "Origin/Core/Event.h"
#include "Origin/Scene/Entity.h"
#include "Origin/Core/KeyEvent.h"
#include "Origin/Scene/EditorCamera.h"
#include "Origin/Scene/Scene.h"

#include "PanelBase.h"

namespace origin
{
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

        void OnUpdate(Timestep ts);
        void Render() override;

        void OnEvent(Event &e) override;
        bool OnMouseButtonPressed(MouseButtonPressedEvent &e);
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

        std::shared_ptr<Framebuffer> m_Framebuffer;

        glm::vec2 m_EditorViewportSize = { 1.0f, 1.0f };
        glm::vec2 m_ViewportBounds[2] = { {0.0f, 0.0f}, {0.0f, 0.0f} };
        glm::ivec2 m_Mouse = { 0, 0 };
        glm::vec2 m_MoveTranslation = { 0.0f, 0.0f };
        int m_SelectedIndex = 0;
    };

}
