// Copyright (c) Evangelion Manuhutu | ORigin Engine

#ifndef SCENE_HIERARCHY_PANEL_HPP
#define SCENE_HIERARCHY_PANEL_HPP

#include "Origin/Scene/Scene.h"
#include "Origin/Scene/Entity.h"

#include "PanelBase.hpp"

namespace origin {

    class SceneHierarchyPanel : public PanelBase
    {
    public:
        SceneHierarchyPanel() = default;
        SceneHierarchyPanel(const Ref<Scene> &scene);

        ~SceneHierarchyPanel();

        Ref<Scene> GetContext() { return m_Scene; }

        Entity SetSelectedEntity(Entity entity);
        Entity GetSelectedEntity();

        void Render() override;
        void OnUpdate(float delta_time) override;

        Entity ShowEntityContextMenu();
        void EntityHierarchyPanel();
        void EntityPropertiesPanel();

        void SetActiveScene(const Ref<Scene> &scene, bool reset = false);
        void DestroyEntity(Entity entity);

        bool IsSceneHierarchyFocused = false;
        bool IsScenePropertiesFocused = false;

        void OnEvent(Event &e) override;

        static SceneHierarchyPanel *GetInstance();

    private:
        bool OnMouseButtonPressed(MouseButtonPressedEvent &e);
        bool OnKeyPressed(KeyPressedEvent &e);

        template<typename T>
        bool DisplayAddComponentEntry(const std::string& entry_name);
        template<typename T, typename UIFunction>
        void DrawComponent(const std::string &name, Entity entity, UIFunction uiFunction);

        void DrawEntityNode(Entity entity, int index = 0);
        void DrawComponents(Entity entity);

        Ref<Scene> m_Scene;
        Ref<Texture2D> m_NoTextureButton;
        bool m_BlendSpacePopUp = false;

        struct BlendSpaceCreateInfo
        {
            std::string name;
            glm::vec2 min_range{0.0f, 0.0f};
            glm::vec2 max_range{10.0f, 10.0f};

            glm::vec2 blend_min_range{ 0.0f, 0.0f };
            glm::vec2 blend_max_range{ 10.0f, 10.0f };
        };

        BlendSpaceCreateInfo m_BlendSpaceInfo;

        void *m_SendData = nullptr;

        Entity m_SelectedEntity = Entity();
    };
}

#endif