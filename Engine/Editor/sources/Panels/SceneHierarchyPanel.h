// Copyright (c) Evangelion Manuhutu | ORigin Engine

#pragma once
#include "Origin/Scene/Scene.h"
#include "Origin/Scene/Entity.h"

#include "PanelBase.h"

namespace origin {

    class SceneHierarchyPanel : public PanelBase
    {
    public:
        SceneHierarchyPanel() = default;
        SceneHierarchyPanel(const std::shared_ptr<Scene> &scene);

        ~SceneHierarchyPanel();

        std::shared_ptr<Scene> GetContext() { return m_Scene; }

        Entity SetSelectedEntity(Entity entity);
        Entity GetSelectedEntity();

        void Render() override;
        void OnUpdate(float delta_time) override;

        void EntityHierarchyPanel();
        void EntityPropertiesPanel();

        void SetActiveScene(const std::shared_ptr<Scene> &scene, bool reset = false);
        void DestroyEntity(Entity entity);

        bool IsSceneHierarchyFocused = false;
        bool IsScenePropertiesFocused = false;

        void OnEvent(Event &e) override;

    private:
        bool OnMouseButtonPressed(MouseButtonPressedEvent &e);
        bool OnKeyPressed(KeyPressedEvent &e);

        template<typename T>
        bool DisplayAddComponentEntry(const std::string& entryName);
        template<typename T, typename UIFunction>
        void DrawComponent(const std::string &name, Entity entity, UIFunction uiFunction);

        void DrawEntityNode(Entity entity, int index = 0);
        void DrawComponents(Entity entity);

        Entity EntityContextMenu();

        std::shared_ptr<Scene> m_Scene;
        std::shared_ptr<Texture2D> m_NoTextureButton;

        Entity m_SelectedEntity = Entity();
    };
}
